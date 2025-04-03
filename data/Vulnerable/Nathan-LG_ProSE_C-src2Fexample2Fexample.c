/**
 * @file example.c
 *
 * @brief Ceci est un exemple de classe active
 * @date 02/05/2019
 * @author Thomas Cravic, Nathan Le Granvallet
 *
 * @copyright Nexelis
 */

#include <pthread.h>
#include "example.h"

/**
 * @brief Compteur d'instances d'Example utilisé pour avoir un nom de boîte aux lettres unique
 */
static int exampleCounter = 0;

/* ----------------------- MAILBOX DEFINITIONS -----------------------*/

/**
 * @def NAME_MQ_BOX
 *
 * Nom des boîtes aux lettres. Chaque BaL aura ce nom suivi du numéro de son instance.
 */
#define NAME_MQ_BOX  "/mboxExample%d"

/**
 * @def NAME_TASK
 *
 * Nom des tâches. Chaque instance aura ce nom suivi du numéro de son instance.
 */
#define NAME_TASK  "ExampleTask%d"


/**
 * @def SIZE_BOX_NAME
 *
 * Taille du nom de la BaL
 */
#define SIZE_BOX_NAME 20

/**
 * @def SIZE_TASK_NAME
 *
 * Taille du nom des tâches
 */
#define SIZE_TASK_NAME 20


/**
 * @def MAX_MESSAGE_LENGTH
 *
 * La taille maximal d'un message, caractère final inclus
 */
#define MAX_MESSAGE_LENGTH 1024


/**
 * @def MQ_MAX_MESSAGES
 *
 * Le nombre maximum de messages que peut contenir la BaL. Par défaut, ne peut être supérieur à 10.
 */
#define MQ_MAX_MESSAGES (10)


/*----------------------- DÉFINITION DES TYPES -----------------------*/

/**
 * @brief Etats de la MAE Example. Tous les états sont préfixés avec S_.
 */
typedef enum {
    S_FORGET = 0, ///< Consommation de l'event, rien ne se passe.
    S_IDLE, ///< Etat Idle
    S_RUNNING, ///< Etat Running
    S_DEATH, ///< Mort de la MAE
    NB_STATE ///< Utile pour compter le nombre d'états possibles. N'est pas un état.
} State;


/**
 * @brief Action de la MAE Example. Toutes les actions sont préfixées avec A_.
 */
typedef enum {
    A_NOP = 0, ///< Aucune action ne se passe
    A_EXAMPLE1_FROM_RUNNING, ///< Action Example1 depuis l'état S_RUNNING
    A_EXAMPLE1_FROM_IDLE, ///< Action Example1 depuis l'état S_IDLE
    A_EXAMPLE2, ///< Action Example2
    A_KILL,  ///< Action tuant la MAE
    NB_ACTION ///< Utile pour compter le nombre d'actions possibles. N'est pas une action
} Action;


/**
 * @brief Événements de la MAE Example. Tous les événements sont préfixés avec E_.
 */
typedef enum {
    E_NOP = 0, ///< Événement nul. Rien ne se passe.
    E_EXAMPLE1, ///< Événement Example1
    E_EXAMPLE2, ///< Événement Example2
    E_KILL, ///< Événement demandant la mort de la MAE
    NB_EVENT ///< Utile pour compter le nombre d'événements possibles. N'est pas un événement.
} Event;


/**
 * @brief Une Transition représente le passage d'un état à un autre de la MAE.
 */
typedef struct {
    State nextState; ///< Prochain état de la MAE
    Action action; ///< Action à effectuer lors de la transition vers nextState
} Transition;

/**
 * @brief Un Message est un message passé dans la boîte aux lettres.
 */
typedef struct {
    Event event; ///< Événement passé dans le Message
    int param; ///< 	Exemple d'un paramètre
    float param2; ///< 	Exemple d'un autre paramètre
} Message;


/**
 * @brief Un Wrapper est utilisé pour envoyer des Message dans la boîte aux lettres.
 */
typedef union {
    Message msg; ///< Message envoyé, interprété comme une structure Message
    char toString[sizeof(Message)]; ///< Message envoyé, interprété comme une chaîne de caractères
} Wrapper;


/**
 * @brief Un Example est un exemple de classe active.
 */
struct Example_t {
    /* NE PAS MODIFIER */
    pthread_t threadId; ///< Identifant Pthread pour l'instance d'Example
    State state; ///< État actuel de la MAE
    Message msg; ///< Utile pour passer des événements et des paramètres à la MAE

    char queueName[SIZE_BOX_NAME]; ///< Nom de la BaL utilisée
    char nameTask[SIZE_TASK_NAME]; ///< Nom de la tâche

    /* VOUS POUVEZ MODIFIER ET AJOUTER */
    int b; ///< Exemple de varaible d'instance
};


/*----------------------- PROTOTYPES DES FONCTIONS STATIQUES -----------------------*/

/*------------- Fonctions d'action -------------*/
/**
 * @brief Appelée quand aucune action n'est nécessaire
 *
 * @param this Adresse d'un Example
 */
static void actionNop(Example * this);


/**
 * @brief Change l'état de la MAE à S_DEATH
 *
 * @param this Adresse d'un Example
 */
static void actionKill(Example * this);


/**
 * @brief Appelée lors de l'événement E_EXAMPLE1 pendant l'état S_IDLE
 *
 * @param this Adresse d'un Example
 */
static void actionExample1FromIdle(Example * this);


/**
 * @brief Appelée lors de l'événement E_EXAMPLE1 pendant l'état S_RUNNING
 *
 * @param this Adresse d'un Example
 */
static void actionExample1FromRunning(Example * this);


/**
 * @brief Appelée lors de l'événement E_EXAMPLE2
 *
 * @param this Adresse d'un Example
 */
static void actionExample2(Example * this);

/*------------- Fonctions de la BàL -------------*/

/**
 * @brief Initilise la BaL
 *
 * @param this Adresse d'un Example
 */
static void MailboxInit(Example * this);


/**
 * @brief Détruit la BaL
 *
 * @param this Adresse d'un Example
 */
static void MailboxClose(Example * this);


/**
 * @brief Envoie un événement stoppant la MAE
 *
 * @param this Adresse d'un Example
 */
static void MailboxSendStop(Example * this);


/**
 * @brief Lit un message dans la BaL
 *
 * @note Fonction bloquante si la BaL est vide
 * @param this Adresse d'un Example
 * @param wrapper Adresse d'un Wrapper
 */
static void MailboxReceive(Example * this, Wrapper * wrapper);

/**
 * @brief Écrit un message dans la BaL
 *
 * @note Fonction bloquante si la BaL est pleine
 *
 * @param this Adresse d'un Example
 * @param msg Message à envoyer
 */
static void MailboxSendMsg(Example * this, Message msg);



/*----------------------- DÉCLARATIONS -----------------------*/

/**
 * @brief Type de pointeurs de fonctions utilisé pour appeler les actions de la MAE
 */
typedef void (*ActionPtr)(Example*);

/**
 * @brief Tableau de pointeurs de fonctions utilisé pour appeler les actions de la MAE
 */
static const ActionPtr actionPtr[NB_ACTION] = {&actionNop, &actionExample1FromRunning, &actionExample1FromIdle, &actionExample2, &actionKill};


/**
 * @brief MAE de la classe Example
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = {
    [S_IDLE][E_EXAMPLE1] = {S_RUNNING,	A_EXAMPLE1_FROM_IDLE},
    [S_RUNNING][E_EXAMPLE1] = {S_RUNNING, A_EXAMPLE1_FROM_RUNNING},
    [S_RUNNING][E_EXAMPLE2] = {S_IDLE, A_EXAMPLE2}
};

/* ----------------------- FONCTIONS D'ACTION ----------------------- */

static void actionExample1FromRunning(Example * this)
{
    TRACE("[ActionEx1FromRunning] - %d\n", this->msg.param);
}


static void actionExample1FromIdle(Example * this)
{
    TRACE("[ActionEx1FromIdle] - %d\n", this->msg.param);
}


static void actionExample2(Example * this)
{
    TRACE("[ActionEx2] - %f\n", this->msg.param2);
}


static void actionNop(Example * this)
{
    TRACE("[ActionNOP]\n");
}

static void actionKill(Example * this)
{
    TRACE("[Action Kill]\n");
    this->state = S_DEATH;
}


/*----------------------- FONCTIONS DES ÉVÉNEMENTS -----------------------*/

/**
 * @brief Envoie un message appelant l'événement E_EXAMPLE1
 *
 * @param this Adresse d'un Example
 * @param param Paramètre de la fonction
 */
void ExampleEventOne(Example * this, int param)
{
    Message message = {
            .event = E_EXAMPLE1,
            .param2 = param
    };

    MailboxSendMsg(this, message);
}

/**
 * @brief Envoie un message appelant l'événement E_EXAMPLE2
 *
 * @param this Adresse d'un Example
 * @param param Paramètre de la fonction
 */
void ExampleEventTwo(Example * this, float param)
{
    Message message = {
            .event = E_EXAMPLE2,
            .param2 = param
    };

    MailboxSendMsg(this, message);
}


/* ----------------------- RUN FUNCTION ----------------------- */


/**
 * @brief Fonction principale de la classe Example
 */
static void ExampleRun(Example * this)
{
    Action action;
    State state;
    Wrapper wrapper;

    TRACE("RUN - Queue name : %s\n", this->queueName);

    while (this->state != S_DEATH) {
        MailboxReceive(this, &wrapper);

        if (wrapper.msg.event == E_KILL) {
            this->state = S_DEATH;

        } else {
            action = stateMachine[this->state][wrapper.msg.event].action;
            TRACE("Action %d\n", action);

            state = stateMachine[this->state][wrapper.msg.event].nextState;
            TRACE("State %d\n", state);

            if (state != S_FORGET) {
                this->msg = wrapper.msg;
                actionPtr[action](this);
                this->state = state;
            }
        }
    }
}

/* ----------------------- NEW START STOP FREE -----------------------*/

Example * ExampleNew()
{
    exampleCounter ++;
    TRACE("ExampleNew function \n");
    Example * this = (Example *) malloc(sizeof(Example));
    this->state = S_IDLE;

    MailboxInit(this);

    int err = sprintf(this->nameTask, NAME_TASK, exampleCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.");

    return this;
}


int ExampleStart(Example * this)
{
    TRACE("ExampleStart function \n");
    int err = pthread_create(&(this->threadId), NULL, (void *) ExampleRun, this);
    STOP_ON_ERROR(err != 0, "Error when creating the thread");

    return 0;
}


int ExampleStop(Example * this)
{
    MailboxSendStop(this);
    TRACE("Waiting for the thread to terminate \n");

    int err = pthread_join(this->threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end");

    return 0;
}


int ExampleFree(Example * this)
{
    TRACE("ExampleFree function \n");
    MailboxClose(this);

    free(this);

    return 0;
}


/* -------------- BOITE AUX LETTRES ------------- */

WEAK
static void MailboxInit(Example * this)
{
    int err = sprintf(this->queueName, NAME_MQ_BOX, exampleCounter);
    TRACE("Defined the Queue name : %s\n", this->queueName);
    STOP_ON_ERROR(err < 0, "Error when setting the queueName");

    TRACE("[MAILBOX] Oppening the mailbox %s\n", this->queueName);
    /* Destroying the mailbox if it already exists */
    mq_unlink(this->queueName);

    /* Creating and opening the mailBox */

    /* Initializes the queue attributes */
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;		// Size of the queue
    attr.mq_msgsize = sizeof(Wrapper);		// Max size of a message
    attr.mq_curmsgs = 0;

    mqd_t mq;
    // Creating the queue
    mq = mq_open(this->queueName, O_CREAT, 0777, &attr);
    STOP_ON_ERROR(mq == -1, "Error when opening the mailbox %s", this->queueName);
    err = mq_close(mq);
    STOP_ON_ERROR(err!=0, "Error when closing the mqueue during initialisation");

}

static void MailboxClose(Example * this)
{
    /* Destruction of the queue */
    int err = mq_unlink(this->queueName);
    STOP_ON_ERROR(err == -1, "Error when unlinking the mailbox %s", this->queueName);

}

static void MailboxSendStop(Example * this)
{
    TRACE("[MAILBOX] Sending stop event to the queue %s\n", this->queueName);
    Message msg = {
            .event = E_KILL
    };

    MailboxSendMsg(this, msg);
}

static void MailboxSendMsg(Example * this, Message msg)
{
    TRACE("[MAILBOX] Sending message to the mailbox %s\n", this->queueName);

    Wrapper wrapper;
    mqd_t mq;
    wrapper.msg = msg;

    mq = mq_open(this->queueName, O_WRONLY);
    STOP_ON_ERROR(mq==-1, "Error when oppening the queue to send a message.");

    int err = mq_send(mq, wrapper.toString, sizeof(Wrapper), 0);
    STOP_ON_ERROR(err == -1, "Error when sending textOnly message");

    err = mq_close(mq);
    STOP_ON_ERROR(err!=0, "Error when closing the writing queue");
}

static void MailboxReceive(Example * this, Wrapper * wrapper)
{
    TRACE("[MAILBOX] Receiving a message from %s\n", this->queueName);
    mqd_t mq;
    mq = mq_open(this->queueName, O_RDONLY);
    STOP_ON_ERROR(mq == -1, "Error when openning the mailbox to read a message : ");

    int err = mq_receive(mq, (char*) wrapper, sizeof(Wrapper), 0);
    STOP_ON_ERROR(err == -1, "Error when receiving a message : ");

    err = mq_close(mq);
    STOP_ON_ERROR(err == -1, "Error when closing the reading mailbox : ");
}
//						↓↓↓VULNERABLE LINES↓↓↓

// 375,14;375,21

// 420,14;420,21

// 375,14;375,21

// 420,14;420,21

