/*    
    This project is started on 06/17/2024
    by Valcyle
    --You can read code for learning but dont use in wrong way!--
    -Valcyle[PRO7169], MCsmash Network (2024)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <direct.h>
#include <errno.h>
#include <stdarg.h>
#include "datas.h"

char* getCurrentTime(FILE* fp);
void getFormattedTime(char *buffer, size_t bufferSize);
void logEvent(FILE* logfile, const char *event, ...);
int createDirectories(const char *path);

int main(int argc, char* argv[]){
    char filename[1024], openname[1024], logfile[32], fullpath[1024];
    char itemname[1024];
    FILE *fp, *logs;
    int inputnumber, ench_id, ench_lvl, enchs;
    unsigned char inputbytes[2];

    if(argc != 1){
        printf("This tool doesn't support command line arguments.\nPlease use by opening exe file.\n");
        exit(0);
    }
    // create logs folder if doesn't exists
    if (createDirectories("logs") != 0) {
        return 1;
    }
    getFormattedTime(logfile, sizeof(logfile));
    sprintf(fullpath, "logs/%s", logfile);
    logs = fopen(fullpath, "w");
    if(logs == NULL){
        printf("Failed to open file.\nStopping program.\n");
        exit(0);
    }

    //opening file
    printf("Input the output filename (ex. teststructure): ");
    scanf("%s", filename);
    strcat(filename, ".mcstructure");
    fp = fopen(filename, "wb");
    //logging
    logEvent(logs, "[File System/INFO]: Opening output file: %s", filename);
    if(fp == NULL){
        printf("Failed to open file.\nStopping program.\n");
        logEvent(logs, "[File System/ERROR]: Failed to open output file\n");
        exit(0);
    }

    //make a things here
    //file first
    fwrite(data1binaryData, sizeof(unsigned char), data1dataSize, fp);
    logEvent(logs, "[Program Core/INFO]: Putted first template data to file\n");
    
    
    //get item input
    printf("Input generate item (ex: minecraft:stick): ");
    scanf("%1023s", itemname);

    //item name
    inputnumber = strlen(itemname);
    inputbytes[0] = inputnumber & 0xFF;       // Low byte (LSB)
    inputbytes[1] = (inputnumber >> 8) & 0xFF; // High byte (MSB)
    fwrite(inputbytes, sizeof(unsigned char), sizeof(inputbytes), fp);
    logEvent(logs, "[Program Core/INFO]: Putted item name length data\n");
    fwrite(itemname, sizeof(unsigned char), inputnumber, fp);
    logEvent(logs, "[Program Core/INFO]: Putted item name data\n");

    //enchant
    fwrite(data2binaryData, sizeof(unsigned char), data2dataSize, fp);
    printf("Input the enchant amount: ");
    scanf("%d", &inputnumber);
    inputbytes[0] = inputnumber & 0xFF;       // Low byte (LSB)
    inputbytes[1] = (inputnumber >> 8) & 0xFF; // High byte (MSB)
    fwrite(inputbytes, sizeof(unsigned char), sizeof(inputbytes), fp);
    logEvent(logs, "[Program Core/INFO]: Putted enchant amount length data\n");
    fwrite("\x00", sizeof(unsigned char), 1, fp);
    enchs = inputnumber;

    //each enchant
    for(int i = 0; i < enchs; i++){
        fwrite("\x00\x02\x02\x00", sizeof(unsigned char), 4, fp);

        printf("Input enchantment ID(ench %d of %d): ", i + 1, enchs);
        scanf("%d", &ench_id);
        printf("Input enchantment level(ench %d of %d)[max 32767]: ", i + 1, enchs);
        scanf("%d", &ench_lvl);

        fwrite("id", sizeof(char), 2, fp);
        inputbytes[0] = ench_id & 0xFF;       // Low byte (LSB)
        inputbytes[1] = (ench_id >> 8) & 0xFF; // High byte (MSB)
        fwrite(inputbytes, sizeof(unsigned char), sizeof(inputbytes), fp);

        fwrite("\x02\x03\x00", sizeof(unsigned char), 3, fp);

        fwrite("lvl", sizeof(char), 3, fp);
        inputbytes[0] = ench_lvl & 0xFF;       // Low byte (LSB)
        inputbytes[1] = (ench_lvl >> 8) & 0xFF; // High byte (MSB)
        fwrite(inputbytes, sizeof(unsigned char), sizeof(inputbytes), fp);

        logEvent(logs, "[Program Core/INFO]: Putted enchant info (%d of %d)\n", i + 1, enchs);
    }

    fwrite(data3binaryData, sizeof(unsigned char), data3dataSize, fp);
    logEvent(logs, "[Program Core/INFO]: Putted template ending data");

    fclose(fp);
    logEvent(logs, "[File System/INFO]: Successfully closed output file\n");
    logEvent(logs, "[Program Core/INFO]: Program finished\n");
    fclose(logs);
    return 0;
}

char* getCurrentTime(FILE* fp) {
    time_t currenttime;
    struct tm *timeinfo;
    char *timestr = (char*)malloc(9 * sizeof(char));

    if (timestr == NULL) {
        perror("Memory allocation failed");
        logEvent(fp, "[Program Core/ERROR]: Memory allocation failue at 'GetCurrentTime'\n");
        exit(1);
    }

    // getting current time
    time(&currenttime);
    timeinfo = localtime(&currenttime);

    // put to buffer
    strftime(timestr, 9, "%H:%M:%S", timeinfo);
    return timestr;
}

void getFormattedTime(char *buffer, size_t bufferSize) {
    time_t rawtime;
    struct tm *timeinfo;

    // getting current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // put to buffer
    strftime(buffer, bufferSize, "%Y-%m-%d-%H-%M-%S.log", timeinfo);
}

void logEvent(FILE* logfile, const char *event, ...) {

    char logMessage[256];
    va_list args;
    va_start(args, event);
    vsnprintf(logMessage, sizeof(logMessage), event, args);
    va_end(args);

    fprintf(logfile, "[%s] %s\n", getCurrentTime(logfile), logMessage);
}

//searched cuz idk how to :D
//creating directory if doesn't exists
int createDirectories(const char *path) {
    char tempPath[256];
    snprintf(tempPath, sizeof(tempPath), "%s", path);

    char *p = tempPath;
    while (*p) {
        if (*p == '\\' || *p == '/') {
            *p = '\0';
            if (_mkdir(tempPath) != 0 && errno != EEXIST) {
                perror("mkdir failed");
                return -1;
            }
            *p = '\\';
        }
        p++;
    }
    return (_mkdir(tempPath) != 0 && errno != EEXIST) ? -1 : 0;
}