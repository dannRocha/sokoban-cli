#ifndef __MAP_SOKOBAN_H__
#define __MAP_SOKOBAN_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "../lib/types.h"


Map loadMap(const char*);
Map parseFilemap(Map);
Levels loadLevels(const char*);
World loadWorlds(const char*);


Map loadMap(const char* filename) {
  const int SIZE = 256;
  
  Map map = {NULL, 0, 0, {NULL, false, 0}};

  FILE *filemap = fopen(filename, "r");
  char buff[SIZE];

  while(fgets(buff, SIZE, filemap)) {
    map.height++;
    
    if(!map.width) {
      map.width = (int) strlen(buff);
    }
    
    
    if(map.width != strlen(buff)) {
      char* messageType = "\033[33;1m⚠  A largura do mapa é definido pelo tamanho da primeira linha do arquivo de mapa\033[0;1m";
      char* messageInfo = (char*) calloc(strlen(filename) + strlen("\033[31;1mError ↴\n  Map: %s, está com tamanho diferente na linha: %d\033[0;1m") + 2, sizeof(char));
      
      sprintf(messageInfo, "\033[31;1mError -> Map: %s, está com tamanho diferente na linha: %d\033[0;1m", filename, map.height);

     
      map.errors.hasError = true;
      map.errors.size++;
      map.errors.list = (Error* ) realloc(map.errors.list, map.errors.size * sizeof(Error));
      map.errors.list[map.errors.size - 1].message = (char*) calloc(strlen(messageInfo), sizeof(char));
      map.errors.list[map.errors.size - 1].type    = (char*) calloc(strlen(messageType), sizeof(char));
      
      strcat(map.errors.list[map.errors.size - 1].type, messageType);
      strcat(map.errors.list[map.errors.size - 1].message, messageInfo);
     
      free(messageInfo); 
    }

    map.field = (char**) realloc(map.field, map.height * sizeof(char*));
    map.field[map.height - 1] = (char*) calloc(map.width, sizeof(char));

    strcpy(map.field[map.height - 1], buff);

  }

  fclose(filemap);
  map = parseFilemap(map);
  return map;
}


Map parseFilemap(Map map) {
  for(int i = 0; i < map.height; i++) {
    for(int j = 0; j < map.width; j++) {
      if(map.field[i][j] == '.') {
        map.field[i][j] = ' ';
      }
    }
  }

  return map;
}




Levels loadLevels(const char* dirname) {
  
  Levels levels = {NULL, NULL, 0};
  struct dirent *list;
  
  DIR *diretory = opendir(dirname);

  if(!diretory) {
    fprintf(stderr, "\033[31;1mError ↴\n  Levels: %s, nao foi possivel lê\033[0;1m", dirname);
    exit(1);
  }
  
  while((list = readdir(diretory)) != NULL) {
    
    if(!strcmp(list->d_name, "..") || !strcmp(list->d_name, "."))
      continue;

    char *path = calloc(strlen(dirname) + strlen(list->d_name), sizeof(char));
    strcat(path, dirname);
    strcat(path, list->d_name);

    if(isdir(path))
      continue;

    free(path);

    levels.total++;
    levels.filenames = (char**) realloc(levels.filenames, levels.total * sizeof(char*));
    levels.paths = (char**) realloc(levels.paths, levels.total * sizeof(char*));
    
    levels.filenames[levels.total - 1] = (char*) calloc(strlen(list->d_name), sizeof(char));
    levels.paths[levels.total - 1] = (char*) calloc(strlen(list->d_name) + strlen(dirname), sizeof(char));
    
    strcpy(levels.filenames[levels.total - 1], list->d_name);
    strcpy(levels.paths[levels.total - 1], dirname);
    strcat(levels.paths[levels.total - 1], list->d_name);

  }

  closedir(diretory);
  
  sort(levels.filenames, levels.total);
  sort(levels.paths, levels.total);

  return levels;
}


World loadWorlds(const char* dirname) {
  
  World world = {NULL, 0};
  struct dirent *list;
  DIR *diretory = opendir(dirname);

  if(!diretory) {
    fprintf(stderr, "\033[31;1mError ↴\n  Worlds: %s, nao foi possivel lê\033[0;1m", dirname);
    exit(1);
  }

  while((list = readdir(diretory)) != NULL) {
    
    Levels levels = {NULL, NULL, 0};

    char *path = calloc(strlen(dirname) + strlen(list->d_name) + 1, sizeof(char));
    strcat(path, dirname);
    strcat(path, list->d_name);
    strcat(path, "/");

    if(!isdir(path) || !strcmp(list->d_name, "..") || !strcmp(list->d_name, "."))
      continue;
    
    levels = loadLevels(path);
   

    world.total++;
    world.levels = (Levels*) realloc(world.levels, world.total * sizeof(Levels));
    world.levels[world.total - 1] = levels;
    
    destroy(&levels, "Levels");
    free(path);
  }
  
  sortWorld(world.levels, world.total);
  
  return world;
}



#endif // __MAP_SOKOBAN_H__