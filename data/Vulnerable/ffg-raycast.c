#include "v3dm_lib.h"
#include "structs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void skip_non_alphanum();
void intersection_sphere();

//MARK: - PARSER FUNCTIONS

//parses the object type from csv
void parse_type(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(sizeof(double)));
  char *character = malloc(sizeof(char));
  sprintf(character, "%c", fgetc(fh));
  while (*character != ',') {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  if (strcmp(str, "sphere") == 0) {
    obj->kind = "SPHERE";
  } else if (strcmp(str, "plane") == 0) {
    obj->kind = "PLANE";
  } else if (strcmp(str, "camera") == 0) {
    obj->kind = "CAMERA";
  } else if (strcmp(str, "light") == 0) {
    obj->kind = "LIGHT";
  } else {
    perror("Error: Object must be of type camera, sphere, plane, or light. Please try again with only these types of objects.");
    exit(EXIT_FAILURE);
  }
  free(character);
  skip_non_alphanum(fh);
}

//moves pointer in file back str(len) positions
void rewind_file(FILE *fh, char *str) {
  fseek(fh, -strlen(str), SEEK_CUR);
}

//checks the string to see if it is alphanum or not
int check_str(FILE *fh, char *str) {
  char *c = malloc(sizeof(char));
  for (int i = 0; i < strlen(str); i++) {
    *c = str[i];
    if ((*c < '0' || *c > '9') && (*c != '.' && *c != '-')) {
      return 0;
    }
  }
  return 1;
}

//parses width and height fields from csv
void parse_field(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 9);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "width") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]\n", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->width);
      if (obj->width <= 0.0) {
        perror("Error: Camera width must be greater than zero. Please try again with value greater than zero.");
      }
    } else {
      perror("Error: Camera width must be a numeric value. Please try again with a numeric value.");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(str, "height") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]\n", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->height);
      if (obj->height <= 0.0) {
        perror("Error: Camera height must be greater than zero. Please try again with a value greater than zero.");
      }
    } else {
      perror("Error: Camera height must be a numeric value. Please try again with a numeric value.");
      exit(EXIT_FAILURE);
    }
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses radius from csv
void parse_radius(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 6);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "radius") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->radius);
      if (obj->radius < 0) {
        perror("Error: Sphere radius must be a positive value. Please try again with a positive radius.");
      }
    } else {
      perror("Error: Sphere radius must be a numeric value. Please try again with a numeric value.");
      exit(EXIT_FAILURE);
    }

  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses position from csv
void parse_position(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 8);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "position") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->position.x);
      skip_non_alphanum(fh);
    } else {
      perror("Error: All x positions must be in numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->position.y);
      skip_non_alphanum(fh);
    } else {
      perror("Error: All y positions must be in numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->position.z);
      skip_non_alphanum(fh);
    } else {
      perror("Error: All z positions must be in numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }
  }
  skip_non_alphanum(fh);
}

//parses color from csv
void parse_color(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 5);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  while (*character != 58) {
    sprintf(character, "%c", fgetc(fh));
    strcat(str, character);
  }

  if (strcmp(str, "color:") == 0) {
    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->color.r);
      if (obj->color.r < 0.0) {
        perror("Error: Color values must be greater than 0.0. Please retry with correct color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: Color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->color.g);
      if (obj->color.g < 0.0) {
        perror("Error: Color values may only range from 0.0 to 1.0. Please retry with the correct color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: Color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->color.b);
      if (obj->color.b < 0.0) {
        perror("Error: Color values may only range from 0.0 to 1.0. Please retry with the correct color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: Color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(str, "diffuse_color:") == 0) {
    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->diffuse_color.r);

      if (obj->diffuse_color.r < 0.0) {
        perror("Error: diffuse_color values may only range from 0.0 to 1.0. Please retry with correct diffuse_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: diffuse_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->diffuse_color.g);
      if (obj->diffuse_color.g < 0.0) {
        perror("Error: diffuse_color values may only range from 0.0 to 1.0. Please retry with the correct diffuse_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: diffuse_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->diffuse_color.b);
      if (obj->diffuse_color.b < 0.0) {
        perror("Error: diffuse_color values may only range from 0.0 to 1.0. Please retry with the correct diffuse_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: diffuse_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(str, "specular_color:") == 0) {
    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->specular_color.r);
      if (obj->specular_color.r < 0.0) {
        perror("Error: specular_color values may only range from 0.0 to 1.0. Please retry with correct specular_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: specular_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->specular_color.g);
      if (obj->specular_color.g < 0.0) {
        perror("Error: specular_color values may only range from 0.0 to 1.0. Please retry with the correct specular_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: specular_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }

    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->specular_color.b);
      if (obj->specular_color.b < 0.0) {
        perror("Error: specular_color values may only range from 0.0 to 1.0. Please retry with the correct specular_color values.");
        exit(EXIT_FAILURE);
      } else {
        skip_non_alphanum(fh);
      }
    } else {
      perror("Error: specular_color values must be of numeric format. Please retry with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else {
    rewind_file(fh, str);
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses theta from csv
void parse_theta(FILE *fh, Object *obj) {
  char *str = malloc(sizeof(char) * 6);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  sprintf(character, "%c", fgetc(fh));
  while (*character != ':') {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "theta") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      double d;
      sscanf(tmp, "%lf", &d);
      obj->theta = d;
    } else {
      perror("Error: Theta values must be of nuermic format. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else {
    obj->theta = 0;
    rewind_file(fh, str);
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses radial from csv
void parse_radial(FILE *fh, Object *obj) {
  char *str = malloc(sizeof(char) * 10);
  char *character = malloc(sizeof(char));
  char *tmp = malloc(100);
  int check = 0;
  sprintf(character, "%c", fgetc(fh));
  while (*character != ':') {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);

  if (strcmp(str, "radial-a2") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->radial.a2);
    } else {
      perror("Error: radial-a2 values must be of numeric format. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(str, "radial-a1") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->radial.a1);
    } else {
      perror("Error: radial-a1 values must be of numeric format. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(str, "radial-a0") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->radial.a0);
    } else {
      perror("Error: radial-a0 values must be of numeric format. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses the normal vector from csv
void parse_normal(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 6);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "normal") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->normal.x);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }

    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->normal.y);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }

    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->normal.z);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//parses light direction from csv
void parse_direction(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 6);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "direction") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->direction.x);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }

    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->direction.y);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }

    skip_non_alphanum(fh);
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->direction.z);
    } else {
      perror("Error: Normal values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

void parse_angular(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 6);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "angular-a0") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->angular);
    } else {
      perror("Error: Angular values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

void parse_reflect(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 12);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "reflectivity") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->reflectivity);
    } else {
      perror("Error: Reflectivity values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else {
    rewind_file(fh, str);
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

void parse_refract(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 12);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "refractivity") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->refractivity);
    } else {
      perror("Error: Refractivity values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else {
    rewind_file(fh, str);
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

void parse_ior(FILE *fh, Object *obj) {
  char *str = (char *) malloc(sizeof(char) * 3);
  char *character = malloc(sizeof(char));
  int check = 0;
  char *tmp = malloc(100);
  sprintf(character, "%c", fgetc(fh));
  while (*character != 58) {
    strcat(str, character);
    sprintf(character, "%c", fgetc(fh));
  }
  skip_non_alphanum(fh);
  if (strcmp(str, "ior") == 0) {
    fscanf(fh, "%[a-z | A-Z | 0-9 | . | -]", tmp);
    check = check_str(fh, tmp);
    if (check == 1) {
      sscanf(tmp, "%lf", &obj->ior);
    } else {
      perror("Error: IOR values must be of numeric type. Please try again with numeric values.");
      exit(EXIT_FAILURE);
    }
  } else {
    rewind_file(fh, str);
  }
  free(character);
  free(tmp);
  skip_non_alphanum(fh);
}

//comprehensive parser that combines all parser helper functions
Scene* parse_csv(FILE *fh, Scene *scene, char *character, Object *objects, Light *lights, int *obj_size, int *l_size) {
  int l_counter = 0;
  int obj_counter = 0;
  Object *new_obj = malloc(sizeof(Object));
  char *str = malloc(100);
  while (*character != EOF) {
    parse_type(fh, new_obj);
    if (strcmp(new_obj->kind, "CAMERA") == 0) {
      parse_field(fh, new_obj);
      parse_field(fh, new_obj);
      skip_non_alphanum(fh);
    } else if (strcmp(new_obj->kind, "SPHERE") == 0) {
      while (strcmp(str, "light") != 0 && strcmp(str, "plane") != 0 && strcmp(str, "camera") != 0 && *character != EOF) {
        sprintf(character, "%c", fgetc(fh));
        strcat(str, character);
        if (strcmp(str, "radius") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_radius(fh, new_obj);
        } else if (strcmp(str, "color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "diffuse_color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "specular_color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "position") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_position(fh, new_obj);
        } else if (strcmp(str, "reflectivity") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_reflect(fh, new_obj);
        } else if (strcmp(str, "refractivity") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_refract(fh, new_obj);
        } else if (strcmp(str, "ior") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_ior(fh, new_obj);
        }
      }
      if (*character != EOF && strcmp(new_obj->kind, "SPHERE") == 0) {
        rewind_file(fh, str);
        strcpy(str, "");
      }
    } else if (strcmp(new_obj->kind, "PLANE") == 0) {
      while (strcmp(str, "light") != 0 && strcmp(str, "sphere") != 0 && strcmp(str, "camera") != 0 && *character != EOF) {
        sprintf(character, "%c", fgetc(fh));
        strcat(str, character);
        if (strcmp(str, "normal") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_normal(fh, new_obj);
        } else if (strcmp(str, "color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "diffuse_color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "specular_color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "position") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_position(fh, new_obj);
        } else if (strcmp(str, "reflectivity") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_reflect(fh, new_obj);
        } else if (strcmp(str, "refractivity") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_refract(fh, new_obj);
        } else if (strcmp(str, "ior") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_ior(fh, new_obj);
        }
      }
      if (*character != EOF && strcmp(new_obj->kind, "PLANE") == 0) {
        rewind_file(fh, str);
        strcpy(str, "");
      }
    } else if (strcmp(new_obj->kind, "LIGHT") == 0) {
      Light *new_light = malloc(sizeof(Light));
      while (strcmp(str, "camera") != 0 && strcmp(str, "plane") != 0 && strcmp(str, "sphere") != 0 && *character != EOF) {
        sprintf(character, "%c", fgetc(fh));
        strcat(str, character);
        if (strcmp(str, "theta") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_theta(fh, new_obj);;
        } else if (strcmp(str, "color") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_color(fh, new_obj);
        } else if (strcmp(str, "radial-a2") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_radial(fh, new_obj);
        } else if (strcmp(str, "radial-a1") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_radial(fh, new_obj);
        } else if (strcmp(str, "radial-a0") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_radial(fh, new_obj);
        } else if (strcmp(str, "position") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_position(fh, new_obj);
        } else if (strcmp(str, "direction") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_direction(fh, new_obj);
        } else if (strcmp(str, "angular-a0") == 0) {
          rewind_file(fh, str);
          strcpy(str, "");
          parse_angular(fh, new_obj);
        }
      }
      if (*character != EOF && strcmp(new_obj->kind, "LIGHT") == 0) {
        rewind_file(fh, str);
        strcpy(str, "");
      } else if (*character == EOF) {
        rewind_file(fh, character);
        strcpy(str, "");
      }

      new_light->position = new_obj->position;
      new_light->color = new_obj->color;
      new_light->radial.a0 = new_obj->radial.a0;
      new_light->radial.a1 = new_obj->radial.a1;
      new_light->radial.a2 = new_obj->radial.a2;
      new_light->theta = new_obj->theta;
      new_light->angular = new_obj->angular;
      new_light->direction = new_obj->direction;

      if (new_light->theta == 0.0) {
        new_light->kind = "POINT";
      } else {
        new_light->kind = "SPOT";
      }
      lights[l_counter] = *new_light;
      l_counter += 1;
    }
    skip_non_alphanum(fh);
    if (!(strcmp(new_obj->kind, "LIGHT") == 0)) {
      if (strcmp(new_obj->kind, "SPHERE") == 0 && new_obj->radius == 0) {
        //skip
      } else if (strcmp(new_obj->kind, "PLANE") == 0) {
        //zero normal check
      }
      objects[obj_counter] = *new_obj;
      obj_counter += 1;
    }
  }
  *obj_size = obj_counter;
  *l_size = l_counter;
  return scene;
}

//skips all non-alphanumeric characters in a file
void skip_non_alphanum(FILE *fh){
  char character;
  character = fgetc(fh);
  if (character == EOF) {
    ungetc(character, fh);
    return;
  }

  //compares character to all non-alphanum characters and cycles through them until an alphanum character is found
  while ((character < '0' && character != '-') || (character > '9' && character < 'A') || (character > 'Z' && character < 'a') || (character > 'z')) {
    if (character == EOF) {
      ungetc(character, fh);
      return;
    }
    character = fgetc(fh);
  }
  //replaces alphanum character that was pulled from the file
  ungetc(character, fh);
}

//returns camera width recursively
double get_width(Object *objects, int obj_size) {
  for(int i = 0; i < obj_size; i++) {
    if(strcmp(objects[i].kind, "CAMERA") == 0) {
      return objects[i].width;
    }
  }
  perror("Error: No camera found in input file. Please use an input file containing a camera object.");
  exit(EXIT_FAILURE);
}

//returns camera height recursively
double get_height(Object *objects, int obj_size) {
  for (int i = 0; i < obj_size; i++) {
    if(strcmp(objects[i].kind, "CAMERA") == 0) {
      return objects[i].height;
    }
  }
  perror("Error: No camera found in input file. Please use an input file containing a camera object.");
  exit(EXIT_FAILURE);
}

//converts argv[x] to a double value recursively
double argv_to_double(char const *str, int index, double result) {
  if (index == strlen(str)) {
    return result;
  } else {
    double updated_result = result;
    char character = str[index];
    if (character > 57 || character < 48) {
      perror("Error: Width and height must be numeric values only. Please retry with different width and height inputs.");
      exit(EXIT_FAILURE);
    }

    //multiply result by 10 to shift values left
    updated_result *= 10;

    //character - '0' returns numerical value of any number 0-9
    updated_result += character - '0';
    argv_to_double(str, index + 1, updated_result);
  }
}

//MARK: -CALCULATIONS

//clamps max value to 255.0 and minimum value to 0.0
void clamp(Color *color) {

  //clamping red
  if (color->r > 1.0) {
    color->r = 1.0;
  } else if (color->r < 0.0) {
    color->r = 0.0;
  } else if (isnan(color->r)) {
    color->r = 0.0;
  }

  //clamping green
  if (color->g > 1.0) {
    color->g = 1.0;
  } else if (color->g < 0.0) {
    color->g = 0.0;
  } else if (isnan(color->g)) {
    color->g = 0.0;
  }

  //clapming blue
  if (color->b > 1.0) {
    color->b = 1.0;
  } else if (color->b < 0.0) {
    color->b = 0.0;
  } else if (isnan(color->b)) {
    color->b = 0.0;
  }
}

//calculates sphere intersections and returns closest one that is in front of the camera
void intersection_sphere(Vector3 *Rd, Vector3 *Ro, double Cx, double Cy, double Cz, double radius, Vector3 *result, double *t_result, int ch) {
  double b, c, t0, t1, x, y, z;
  Vector3 _subtracted;
  Vector3 *subtracted = &_subtracted;
  v3dm_assign(Cx, Cy, Cz, subtracted);
  v3dm_subtract(Ro, subtracted, subtracted);
  b = 2 * ((Rd->x * subtracted->x) + (Rd->y * subtracted->y) + (Rd->z * subtracted->z));
  c = (subtracted->x * subtracted->x) + (subtracted->y * subtracted->y) + (subtracted->z * subtracted->z) - (radius * radius);
  t0 += (-b - (sqrt((b*b) - (4*c))))/2.0;
  t1 += (-b + (sqrt((b*b) - (4*c))))/2.0;
  if (!isnan(t0) && !isnan(t1)) {
    if (t0 >= 0.0 && t1 >= 0.0) {
      if (t0 < t1) {
        *t_result = t0;
        v3dm_scale(Rd, t0, result);
        v3dm_add(Ro, result, result);

      } else if (t1 <= t0) {
        *t_result = t1;
        v3dm_scale(Rd, t1, result);
        v3dm_add(Ro, result, result);

      }
    } else if (t1 >= 0.0) {
      *t_result = t1;
      v3dm_scale(Rd, t1, result);
      v3dm_add(Ro, result, result);

    } else if (t0 >= 0.0) {
      *t_result = t0;
      v3dm_scale(Rd, t0, result);
      v3dm_add(Ro, result, result);

    }
  } else if (isnan(t1) && !isnan(t0)) {
    *t_result = t0;
    v3dm_scale(Rd, t0, result);
    v3dm_add(Ro, result, result);
  } else if (isnan(t0) && !isnan(t0)){
    *t_result = t1;
    v3dm_scale(Rd, t1, result);
    v3dm_add(Ro, result, result);
  } else {
    *t_result = INFINITY;
    v3dm_assign(NAN, NAN, NAN, result);
  }
}

//calculates plane intersection and returns closest intersection in front of the camera
void intersection_plane(Vector3 *Ro, Vector3 *Rd, struct Vector3 norm, struct Vector3 point, Vector3 *result, double *t_result) {
  double dotRo, dotRd, d, t;
  Vector3 tmpro, tmprd, n_unit, origin;
  v3dm_unit(&norm, &n_unit);
  d = v3dm_pointToPlaneDistance(n_unit, point, Ro);
  if (!isnan(d) && d > 0) {
    v3dm_unit(Ro, &tmpro);
    v3dm_unit(Rd, &tmprd);
    dotRo = v3dm_dot(&n_unit, &tmpro);
    dotRd = v3dm_dot(&n_unit, &tmprd);
    if (dotRd != 0) {
      t = -(dotRo + d) / (dotRd);
      if (t > 0) {
        *t_result = t;
        v3dm_scale(Rd, t, result);
        v3dm_add(Ro, result, result);
        return;
      }
    }
  }
  *t_result = INFINITY;
  v3dm_assign(NAN, NAN, NAN, result);
}

//calculates intersection with light
void intersection_light(Vector3 *Ro, Vector3 *light_pos, double *t) {
  double xdiv, ydiv, zdiv;
  Vector3 Rd, unit_Rd;
  v3dm_subtract(light_pos, Ro, &Rd);
  v3dm_unit(&Rd, &unit_Rd);
  *t = Rd.x/unit_Rd.x;
}

void castARay_primitive(Object *objects, Vector3 *Ro, Vector3 *Rd, ObjectPlus *result, int obj_size, int ch, Light *light) {
  double closest_distance, distance, t_result, t_closest;
  closest_distance = NAN;
  Vector3 _closest_intersection;
  Vector3 *closest_intersection = &_closest_intersection;
  Color *color;
  Object object;
  t_closest = INFINITY;
  for(int x = 0;  x < obj_size; x++) {
    object = objects[x];
    if (strcmp(object.kind, "CAMERA") != 0) {;
      if (strcmp(object.kind, "SPHERE") == 0) {
        intersection_sphere(Rd, Ro, object.position.x, object.position.y, object.position.z, object.radius, result->intersection, &t_result, ch);
      } else if (strcmp(object.kind, "PLANE") == 0) {
        intersection_plane(Ro, Rd, object.normal, object.position, result->intersection, &t_result);
      }

      //check t-value instead of distance
      if (t_result > 0.0 && t_result < INFINITY) {
        if (t_result < t_closest) {
          v3dm_assign(result->intersection->x, result->intersection->y, result->intersection->z, closest_intersection);
          result->object = object;
          t_closest = t_result;
          result->t = t_result;
          result->valid = 1;
        }
      }
    }

    if (ch == 1) {
      intersection_light(Ro, &light->position, &t_result);
      if (t_result > 0.0 && t_result < INFINITY) {
        if (t_result < t_closest) {
          result->t = t_result;
          result->valid = 1;
        } else {
          result->valid = 2;
        }
      }
    }
  }

  v3dm_assign(closest_intersection->x, closest_intersection->y, closest_intersection->z, result->intersection);
  return;
}

//returns color of closest object in front of the camera, or black if no object is closest in front of the camera
void castARay(Object *objects, Vector3 *Ro, Vector3 *Rd, Light *lights, int recursive, Color *final_color, int l_size, int obj_size) {
  Vector3 _intersection;
  Vector3 *intersection = &_intersection;
  Vector3 _hit;
  Vector3 *hit = &_hit;
  ObjectPlus _result;
  ObjectPlus *result = &_result;
  Vector3 _tmpro;
  Vector3 *tmpro = &_tmpro;
  Vector3 _Ro2;
  Vector3 *Ro2 = &_Ro2;
  Vector3 _Rd2;
  Vector3 *Rd2 = &_Rd2;
  Vector3 _v;
  Vector3 *v = &_v;
  Vector3 _r;
  Vector3 *r = &_r;
  Vector3 _l;
  Vector3 *l = &_l;
  Vector3 _n;
  Vector3 *n = &_n;
  Vector3 scaled_normal;
  Color _light_color;
  Color *light_color = &_light_color;
  Light light = lights[0];
  Object _new_object;
  Object *new_object = &_new_object;
  result->intersection = &_intersection;
  result->valid = 0;
  double epsilonx, epsilony, epsilonz, t_new;
  double n_dot_l;

  //TODO: new_object potentially not being assigned correctly...

  castARay_primitive(objects, Ro, Rd, result, obj_size, 0, &light);

  if (result->valid == 0) {
    return;
  }

  v3dm_assign(result->intersection->x, result->intersection->y, result->intersection->z, Ro2);
  *new_object = result->object;
  t_new = result->t;
  result->t = INFINITY;

  v3dm_subtract(Ro2, Ro, v);
  v3dm_unit(v, v);

  v3dm_subtract(&light.position, Ro2, l);
  v3dm_unit(l, l);

  if (strcmp(new_object->kind, "PLANE") == 0) {
    v3dm_unit(&new_object->normal, &new_object->normal);
    v3dm_reflect(l, &new_object->normal, r);
    n_dot_l = v3dm_dot(l, &new_object->normal);
    v3dm_scale(&new_object->normal, 0.1, &scaled_normal);
    v3dm_add(Ro2, &scaled_normal, &scaled_normal);
  } else if (strcmp(new_object->kind, "SPHERE") == 0) {
    v3dm_subtract(Ro2, &new_object->position, &new_object->normal);
    v3dm_copy(&new_object->normal, &scaled_normal);
    v3dm_unit(&new_object->normal, &new_object->normal);
    v3dm_reflect(l, &new_object->normal, r);
    v3dm_unit(r, r);
    n_dot_l = v3dm_dot(l, &new_object->normal);
    v3dm_scale(&new_object->normal, 0.31, &scaled_normal);
    v3dm_add(Ro2, &scaled_normal, &scaled_normal);
    //v3dm_scale(&scaled_normal, 1.2, &scaled_normal);
    //v3dm_add(&scaled_normal, &new_object->position, &scaled_normal);
  }


  for (int x = 0; x < l_size; x++) {

    v3dm_subtract(&light.position, &scaled_normal, Rd2);
    v3dm_unit(Rd2, Rd2);

    castARay_primitive(objects, &scaled_normal, Rd2, result, obj_size, 1, &light);
    double t;
    intersection_light(&scaled_normal, &light.position, &t);

    //shadows
    if (t > result->t && result->valid) {
      continue;
    }

    light_color = &light.color;

    //radial attenuation
    double dl = v3dm_pointToPointDistance(&light.position, Ro2);
    double f_rad = 1.0;
    if (dl < 1000) {
      f_rad = 1/(light.radial.a2 * dl * dl + light.radial.a1 * dl + light.radial.a0);
    }

    //angular attenuation
    double f_ang;
    Vector3 _vo, _vl;
    Vector3 *vo = &_vo;
    Vector3 *vl = &_vl;
    v3dm_subtract(Ro2, &light.position, vo);
    v3dm_unit(vo, vo);
    v3dm_subtract(&light.direction, &light.position, vl);
    v3dm_unit(vl, vl);

    if (strcmp(light.kind, "SPOT") == 0) {
      double vo_dot_vl = v3dm_dot(vo, vl);
      double alpha = acos(vo_dot_vl) * (180/M_PI);
      if (alpha > light.theta) {

        f_ang = 0.0;
      } else {
        f_ang = pow(vo_dot_vl, light.angular);
      }
    } else {
      f_ang = 1.0;
    }
    //calculate the diffuse reflection
    double Idiff_r, Idiff_g, Idiff_b;
    if (n_dot_l > 0.0) {
      Idiff_r = new_object->diffuse_color.r * light_color->r * (n_dot_l);
      Idiff_g = new_object->diffuse_color.g * light_color->g * (n_dot_l);
      Idiff_b = new_object->diffuse_color.b * light_color->b * (n_dot_l);
    } else {
      Idiff_r = 0.0;
      Idiff_g = 0.0;
      Idiff_b = 0.0;
    }

    //calculate the specular reflection
    double Ispec_r = 0.0;
    double Ispec_g = 0.0;
    double Ispec_b = 0.0;
    if (v3dm_dot(v,r) > 0.0) {

      Ispec_r = new_object->specular_color.r * light_color->r * pow(v3dm_dot(v, r), 20);
      Ispec_g = new_object->specular_color.g * light_color->g * pow(v3dm_dot(v, r), 20);
      Ispec_b = new_object->specular_color.b * light_color->b * pow(v3dm_dot(v, r), 20);
    } else {
      Ispec_r = 0.0;
      Ispec_g = 0.0;
      Ispec_b = 0.0;
    }

    final_color->r += (1.0 - new_object->reflectivity) * (f_ang * f_rad * (Idiff_r + Ispec_r));
    final_color->g += (1.0 - new_object->reflectivity) * (f_ang * f_rad * (Idiff_g + Ispec_g));
    final_color->b += (1.0 - new_object->reflectivity) * (f_ang * f_rad * (Idiff_b + Ispec_b));


    light = lights[x+1];
  }


  //recursive reflection handling
  if (recursive) {
    if (new_object->reflectivity > 0.0) {
      //TODO: Color not showing up correctly
      //TODO: SPHERE REFLECTIVITY ALTERS WHOLE IMAGE
      Color _new_color;
      Color *new_color = &_new_color;
      new_color->r = 0.0;
      new_color->g = 0.0;
      new_color->b = 0.0;
      castARay(objects, &scaled_normal, r, lights, recursive-1, new_color, l_size, obj_size);
      final_color->r += new_object->reflectivity * new_color->r;
      final_color->g += new_object->reflectivity * new_color->g;
      final_color->b += new_object->reflectivity * new_color->b;
    }
  }
  return;
}

//grabs color values of closest objects in front of camera and writes them to an array to return when complete
int* render(double width, double height, double xRes, double yRes, Scene *scene, int l_size, int obj_size) {
  double x, y;
  int *colors;
  Vector3 *Pij, *Rd, *Ro;
  Color _color;
  Color *color = &_color;
  Ro = malloc(sizeof(Vector3));
  Pij = malloc(sizeof(Vector3));
  Rd = malloc(sizeof(Vector3));
  color = malloc(sizeof(Color));
  colors = malloc(sizeof(int) * xRes * yRes * 3);
  Object _reflecting_obj;
  Object *reflecting_obj = &_reflecting_obj;
  v3dm_assign(0, 0, 0, Ro);

  int counter = 0;
  for (int j = 0; j < yRes; j++) {
    //-y for some reason flips it the right way up
    y =  (height/2) - (j * (height/yRes)) - (0.5 * (height/yRes));
    for (int i = 0; i < xRes; i++) {
      x = -(width/2) + (i * (width/xRes)) + (0.5 * (width/xRes));
      v3dm_assign(x, y, -1, Pij);
      v3dm_add(Pij, Ro, Rd);
      v3dm_unit(Rd, Rd);
      castARay(scene->objects, Ro, Rd, scene->lights, 7, color, l_size, obj_size);
      clamp(color);
      colors[counter] = (int) (color->r * 255);
      counter++;
      colors[counter] = (int) (color->g * 255);
      counter++;
      colors[counter] = (int) (color->b * 255);
      counter++;
      color->r = 0.0;
      color->g = 0.0;
      color->b = 0.0;
    }
  }
  return colors;
}

//writes all data in the colors array to a file
void write_to_file(FILE *fh, int *array, int xRes, int yRes) {
  for (int i = 0; i < xRes * yRes * 3; i++) {
    fprintf(fh, "%d\n", array[i]);
  }
}

//checks the extension on a file against a passed value
int checkExtension(char *fileName, char *extension) {
  char character, ext_char;
  for (int i = 1; i < strlen(extension); i++) {
    ext_char = extension[strlen(extension) - i];
    character = fileName[strlen(fileName) - i];
    if (!(character == ext_char)) {
      return -1;
    }
  }
}

//MARK: -MAIN FUNCTION
int main(int argc, char const *argv[]) {
  printf("PROGRAM START\nINITIAL CONDITIONS CHECK\n");

  //initial check variables
  int test_index = 4;
  char *test_str;
  if (sizeof(argv[3]) >= sizeof(argv[4])) {
    test_str = (char *) malloc(sizeof(argv[3]));
  } else {
    test_str = (char *) malloc(sizeof(argv[4]));
  }
  test_str = (char *) argv[3];
  char *test_char;
  char *ext = (char *) malloc(sizeof(char) * 4);

  //initial checks
  if (argc != 5) {
    perror("Error: This program requires 4 arguments. Please retry with the correct number of arguments");
    exit(EXIT_FAILURE);
  }

  if (!fopen(argv[3], "r")) {
    perror("Error: Please select a different input file, as this one does not exist.");
    exit(EXIT_FAILURE);
  }

  if (checkExtension(test_str, ".csv") == -1) {
    perror("Error: Please choose a new input file with a .csv extension.");
    exit(EXIT_FAILURE);
  }
  test_str = (char *) argv[4];
  if (checkExtension(test_str, ".ppm") == -1) {
    perror("Error: Please choose a new output file with a .ppm extension.");
    exit(EXIT_FAILURE);
  }

  printf("CHECKS PASSED\nSETTING UP VARIABLES\n");

  //variables for setup later
  FILE *input_file, *output_file;
  char *character = malloc(sizeof(char));
  double width, height, xRes, yRes;
  double index = 0;
  int *colors;

  //setting up some variables
  int l_size, obj_size;
  Object *object = malloc(sizeof(Object));
  struct Object objects[128];
  struct Light lights[128];
  Scene *scene = malloc(sizeof(Scene));
  Light *light = malloc(sizeof(Scene));
  Object *result_object = malloc(sizeof(Object));
  input_file = fopen(argv[3], "r");
  sprintf(character, "%c", fgetc(input_file));

  xRes = argv_to_double(argv[1], index, xRes);
  yRes = argv_to_double(argv[2], index, yRes);
  scene->objects = objects;
  scene->lights = lights;


  //replacing character in file for parser to work correctly
  ungetc(*character, input_file);

  printf("VARIABLE SETUP COMPLETE\n");

  printf("PARSING FILE\n");

  //parsing to return object
  scene = parse_csv(input_file, scene, character, objects, lights, &obj_size, &l_size);
  fclose(input_file);

  printf("FILE PARSED\n");

  //rewinding linked_list then getting width & height from camera and rewinding again
  //scene = rewind_scene(scene);

  //real world width & height values
  width = get_width(scene->objects, obj_size);
  height = get_height(scene->objects, obj_size);

  //rewinding linked list to be reused
  //scene = rewind_scene(scene);
  printf("RENDERING\n");

  //allocating memory for array to accept all color values
  colors = malloc(xRes * yRes * sizeof(double) * 3);
  colors = render(width, height, xRes, yRes, scene, l_size, obj_size);

  printf("RENDER COMPLETE\n");

  printf("WRITING TO FILE\n");
  //setting up image file
  output_file = fopen(argv[4], "w");
  fprintf(output_file, "%s%d\n%s\n%d\n%d\n%d\n", "P", 3, "#superfluous comment", (int) xRes, (int) yRes, 255);

  printf("WRITE COMPLETE\n");

  //writing to image file
  write_to_file(output_file, colors, xRes, yRes);
  fclose(output_file);

  printf("PROGRAM FINISHED RUNNING\n");

  return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 17,2;17,9

// 19,4;19,10

// 20,4;20,11

// 61,2;61,9

// 63,4;63,10

// 64,4;64,11

// 103,2;103,9

// 105,4;105,10

// 106,4;106,11

// 134,2;134,9

// 136,4;136,10

// 137,4;137,11

// 181,4;181,11

// 182,4;182,10

// 338,2;338,9

// 340,4;340,10

// 341,4;341,11

// 370,2;370,9

// 372,4;372,10

// 373,4;373,11

// 416,2;416,9

// 418,4;418,10

// 419,4;419,11

// 463,2;463,9

// 465,4;465,10

// 466,4;466,11

// 509,2;509,9

// 511,4;511,10

// 512,4;512,11

// 535,2;535,9

// 537,4;537,10

// 538,4;538,11

// 563,2;563,9

// 565,4;565,10

// 566,4;566,11

// 591,2;591,9

// 593,4;593,10

// 594,4;594,11

// 628,8;628,15

// 629,8;629,14

// 632,10;632,16

// 636,10;636,16

// 640,10;640,16

// 644,10;644,16

// 648,10;648,16

// 652,10;652,16

// 656,10;656,16

// 660,10;660,16

// 666,8;666,14

// 670,8;670,15

// 671,8;671,14

// 674,10;674,16

// 678,10;678,16

// 682,10;682,16

// 686,10;686,16

// 690,10;690,16

// 694,10;694,16

// 698,10;698,16

// 702,10;702,16

// 708,8;708,14

// 713,8;713,15

// 714,8;714,14

// 717,10;717,16

// 721,10;721,16

// 725,10;725,16

// 729,10;729,16

// 733,10;733,16

// 737,10;737,16

// 741,10;741,16

// 745,10;745,16

// 751,8;751,14

// 754,8;754,14

// 1303,2;1303,9

