#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"
#include "cmdline.h"
#include "config.h"
#include "valor.h"
#include "readbam.h"
#include "readbed.h"
#include "statistics.h"
#include "recovermolecules.h"
#include "interval10X.h"
#include "structural_variation.h"
#include "vector.h"
#include "bitset.h"
#include "clique.h"
#include "cluster.h"
#include "cnv.h"
#include "sonic/sonic.h"
#include "graph.h"
#include "progress.h"
#include <omp.h>
int CUR_CHR = -1;
FILE *logFile = NULL;
double CLONE_MEAN;
double CLONE_STD_DEV;

int main( int argc, char **argv){

    parameters * params = init_params();
    if(parse_command_line(argc,argv,params)){
        return 0;
    }
#ifdef _OPENMP
    omp_set_num_threads(params->threads);
#endif
    sv_type svs_to_find =  params->svs_to_find;

    char *bamname = params->bam_file;;

    int i,j,k;
    time_t rawtime;
    struct tm *timeinfo;
    vector_t **regions; // Vector of Inverval_10X
    vector_t *groups;   // Vector of Interval_10X
    vector_t **variations; //Vector of sv_t
    vector_t **clusters;   //Vector of cluster_t




    mkdir(params->outprefix, 0755 );
    char *out_file_path = malloc((strlen("/predicted_svs.bedpe")+strlen(params->outprefix)+1)*sizeof(char));
    sprintf(out_file_path,"%s/predicted_svs.bedpe",params->outprefix);
    FILE *outbedfile = fopen(out_file_path,"w+");
    free(out_file_path);
    time( &rawtime);

    timeinfo = localtime( &rawtime);


    printf("\n\nVALOR: Variation with LOng Range\n");
    printf("Version: %s\n", VALOR_VERSION);
    printf("Build Date: %s\n",BUILD_DATE);
    printf("Output Directory: %s\n",params->outprefix);
    printf("Logfile name: %s\n", params->logfile);


    sonic *snc = sonic_load(params->sonic_file);
    printf("Reading BAM file: %s\n", bamname);

    logFile = safe_fopen(params->logfile,"w+");                                                                    

    char *molecule_bed_path = malloc((strlen(params->outprefix) + strlen("/molecules.bed") + 1) * sizeof(char));
    sprintf(molecule_bed_path,"%s/molecules.bed",params->outprefix);                                            
    //////                                                                                                        
    //
    bam_info *in_bams = get_bam_info(snc);

    fprintf( logFile, "#CreationDate=%d.%d.%d\n\n",
            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday); 
    in_bams->sample_name = NULL;


    bam_stats *stats = calculate_bam_statistics(in_bams, bamname, READ_SAMPLE_SIZE);

    //	bam_vector_pack **reads = read_10X_bam(in_bams,bamname,snc);
    bam_vector_pack **reads = malloc(sizeof(bam_vector_pack) * snc->number_of_chromosomes);//read_10X_bam(in_bams,bamname,snc);

    regions = getMem(sizeof(vector_t *) * snc->number_of_chromosomes);
    variations = getMem(sizeof(vector_t *) * snc->number_of_chromosomes);
    clusters = getMem(sizeof(vector_t *) * snc->number_of_chromosomes);
    int first_skipped = 0;
    for( i = 0; i < params->chromosome_count ;i++){

        CUR_CHR = i;
        reads[i] = read_10X_chr(in_bams,bamname,snc,i,stats);
        if(reads[i]->concordants->size == 0){
            destroy_bams(reads[i]);
            printf("No reads for chromosome %s %s %s.\r",
                    snc->chromosome_names[first_skipped],
                    (i-first_skipped==1?"and":"to"),
                    snc->chromosome_names[i]);
            continue;
        }
        bit_set_set_bit(get_bam_info(NULL)->chro_bs,i,1);
        printf("\nFinding structural variants in chromosome %s\n",snc->chromosome_names[i]);
        first_skipped = i+1;

        printf("Recovering split molecules..\n");
        regions[i] = recover_molecules(reads[i]->concordants);

        if(params->svs_to_find & SV_TRANSLOCATION){
            append_molecules_to_bed(regions[i],molecule_bed_path);
        }
        in_bams->depths[i] = make_molecule_depth_array(regions[i],snc,i);

        in_bams->depth_mean[i] = make_global_molecule_mean(in_bams->depths[i],snc,i);
        in_bams->depth_std[i] = make_global_molecule_std_dev(in_bams->depths[i],snc,i,in_bams->depth_mean[i]);
        in_bams->depth_std[i] = MIN(in_bams->depth_std[i],in_bams->depth_mean[i]/2);
        printf("Global molecule depth mean: %lf\nGlobal molecule depth standard deviation: %lf\n",
                in_bams->depth_mean[i],in_bams->depth_std[i]);

        VALOR_LOG("chr: %s\nmolecule mean depth: %lf\nmolecule std-dev depth: %lf\n", snc->chromosome_names[i], in_bams->depth_mean[i], in_bams->depth_std[i]);
        VALOR_LOG("Initial molecule count: %zu\n",regions[i]->size);
        filter_molecules(regions[i],snc,i);

        VALOR_LOG("Filtered molecule count: %zu\n",regions[i]->size);
        qsort(regions[i]->items,regions[i]->size,sizeof(void*),interval_start_comp);  
        groups = group_overlapping_molecules(regions[i]);
        groups->rmv = &vector_free;

        CLONE_MEAN = molecule_mean(regions[i]);
        CLONE_STD_DEV = molecule_group_std(groups,CLONE_MEAN);
        VALOR_LOG("Molecule size mean: %lf\nMolecule size std-dev: %lf\n", CLONE_MEAN, CLONE_STD_DEV);
        vector_free(groups);

        qsort(regions[i]->items,regions[i]->size,sizeof(void*),barcode_comp);  

        printf("Molecule Count: %zu\tMolecule mean: %lf\tMolecule std-dev: %lf\n",regions[i]->size,CLONE_MEAN,CLONE_STD_DEV);

        vector_t *split_molecules = discover_split_molecules(regions[i]);
        vector_free(regions[i]);

        VALOR_LOG("Split molecule candidate count: %zu\n",split_molecules->size);

        printf("Matching split molecules\n");
        variations[i] = find_svs(split_molecules,svs_to_find);

        VALOR_LOG("Matched split molecule pair count: %zu\n", variations[i]->size);

        vector_free(split_molecules);
        printf("%zu candidate variations are made\n",variations[i]->size);
        update_sv_supports_b(variations[i],
                reads[i]);
        variations[i]->REMOVE_POLICY = REMP_LAZY;

        vector_filter(variations[i],sv_is_proper);

        VALOR_LOG("Structural variation candidate count: %zu\n",variations[i]->size);

        printf("%zu candidate variations are left after filtering\n",variations[i]->size);
#if FILTER1XK //If number of variations are more than MAX_INVERSIONS_IN_GRAPH, do random selection
        if(variations[i]->size > MAX_INVERSIONS_IN_GRAPH){
            srand(time(NULL));
            variations[i]->REMOVE_POLICY=REMP_LAZY;
            for(k=0;k<variations[i]->size;k++){
                if(rand()%variations[i]->size>MAX_INVERSIONS_IN_GRAPH){
                    vector_remove(variations[i],k);
                }
            }
            vector_defragment(variations[i]);

            printf("%zu candidate variations are left after random selection to decrease size\n",variations[i]->size);
        }
#endif

        variations[i]->REMOVE_POLICY=REMP_SORTED;

        graph_t *sv_graph = make_sv_graph(variations[i]);
        /*
           fff = fopen("sv_graph_out.out","w+");
           graph_print(sv_graph,fff);
           fclose(fff);

*/
        printf("Finding SV Clusters\n\n");
        clusters[i] = vector_init(sizeof(sv_cluster),50);
        vector_set_remove_function(clusters[i],&sv_cluster_destroy);



        graph_trim(sv_graph);

        vector_t *comps = g_dfs_components(sv_graph);

        if(comps->size == 0){continue;}

        for(k=0;k<comps->size;k++){
            vector_t *garbage = vector_get(comps,k);
            size_t initial_size = garbage->size;
            qsort(garbage->items, garbage->size, sizeof(sv_t *),&sv_comp);
            sv_type _type = ((sv_t *)vector_get(garbage,0))->type;
            if(garbage->size < what_is_min_cluster_size(_type)){continue;}
            graph_t *garbage_graph = sv_graph;//= make_sv_graph(garbage);
            int iteration_no = 0;
            while(garbage_graph->number_of_items > 2){
                if(garbage->size < initial_size /4){break;}

                clique_t *c = clique_find_clique(garbage_graph,garbage,0,QCLIQUE_LAMBDA,QCLIQUE_GAMMA);

                if(c==NULL||c->v_prime<=0){clique_free(c);break;}
                sv_cluster *svc_garbage = sv_cluster_make(c);
                clique_free(c);

                if(svc_garbage==NULL){break;}
                if(svc_garbage->items->size < what_is_min_cluster_size(_type)){
                    sv_graph_reset(garbage_graph);
                    sv_cluster_graph_fix(svc_garbage,garbage,sv_graph);
                    sv_cluster_destroy(svc_garbage);
                    continue;
                }


                sv_graph_reset(garbage_graph);

                sv_cluster_graph_fix(svc_garbage,garbage,garbage_graph);



                vector_soft_put(clusters[i],svc_garbage);
                iteration_no++;
            }
            //			vector_free(garbage);
            //	graph_free(garbage_graph);
        }

        printf("Clustering is finished, found %zu variant clusters\n",clusters[i]->size);

        vector_free(comps);

        qsort(clusters[i]->items, clusters[i]->size, sizeof( void*), cluster_comp);
        printf("Printing variant calls\n");
        for(j=0;j<clusters[i]->size;j++){
            sv_cluster *svc = vector_get(clusters[i],j);


            sv_t *first = vector_get(svc->items,0);
            if(svc->items->size < what_is_min_cluster_size(first->type)){continue;}


            double mean_depth = 0;
            if(first->type == SV_DUPLICATION || first->type == SV_INVERTED_DUPLICATION){
                if(first->orientation == DUP_FORW_COPY){
                    mean_depth = get_depth_region(in_bams->depths[i],svc->break_points->start1,svc->break_points->end1);
                }else if(first->orientation == DUP_BACK_COPY){
                    mean_depth = get_depth_region(in_bams->depths[i],svc->break_points->start1,svc->break_points->end1);
                }

                if(mean_depth < 1.5 * in_bams->depth_mean[i]){ continue;}
                //if(mean_depth < in_bams->depth_mean[i] + 1.25 * in_bams->depth_std[i]){ continue;}
            }else if (first->type == SV_DELETION){
                mean_depth = get_depth_region(in_bams->depths[i],first->AB.end1, first->AB.start2);
            }else {
                mean_depth = get_depth_region(in_bams->depths[i],first->AB.end1,first->CD.start1)/2 + get_depth_region(in_bams->depths[i],first->AB.end2,first->CD.start2)/2;
            }

            fprintf(outbedfile,"%s\t%d\t%d\t%s\t%d\t%d\t%s\t%zu\t%d\t%lf\n",
                    snc->chromosome_names[i],
                    svc->break_points->start1,
                    svc->break_points->end1,
                    snc->chromosome_names[i],
                    svc->break_points->start2,
                    svc->break_points->end2,
                    sv_type_name(first->type),
                    svc->items->size,
                    svc->supports[0]+svc->supports[1],
                    mean_depth       
                   );
        }

        fflush(outbedfile);

        if((svs_to_find & SV_TRANSLOCATION) == 0){
            destroy_bams(reads[i]);
            free(in_bams->depths[i]);
        }
        vector_free(variations[i]);
        vector_free(clusters[i]);
        graph_free(sv_graph);

        printf("Reading next chromosome\n");
    }
    printf("\n");

    if(svs_to_find & SV_TRANSLOCATION){
        printf("Looking for translocations.\n");
        printf("Reading from temp molecule file.\n");
        vector_t **molecules = read_molecules_from_bed(molecule_bed_path);
        vector_t *variants = find_interchromosomal_events(molecules,reads);
        for(k=0;k<variants->size;k++){
            vector_t *sub_vec = vector_get(variants,k);
            for(i=0;i<sub_vec->size;i++){
                ic_sv_bed_print(outbedfile,vector_get(sub_vec,i));
            }
        }

        vector_free(variants);
        for(k=0;k<snc->number_of_chromosomes;k++){
            vector_free(molecules[k]);
        }
        free(molecules);	
    }

    free(molecule_bed_path);
    free(in_bams->depths);
    free(in_bams->depth_mean);
    free(in_bams->depth_std);
    bit_set_free(in_bams->chro_bs);
    freeMem(in_bams, sizeof(bam_info));
    free(stats);
    freeMem(clusters, sizeof(vector_t *) * snc->number_of_chromosomes);
    freeMem(variations,sizeof(vector_t *) * snc->number_of_chromosomes);
    free(regions);
    free(reads);
    free_sonic(snc);
    fclose(logFile);
    fclose(outbedfile);
    free_params(params);
    return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 78,4;78,11

// 57,4;57,11

