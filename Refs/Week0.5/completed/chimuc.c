#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORDS 1000
#define MAX_LINE_LENGTH 256
#define MAX_STOPWORDS 100

typedef struct {
    char word[50];
    int count;
    char lines[256]; // Danh sách dòng xuất hiện
} IndexEntry;

IndexEntry indexTable[MAX_WORDS];
int indexCount = 0;

char stopWords[MAX_STOPWORDS][50];
int stopWordCount = 0;

// Chuyển một từ thành chữ thường
void toLowerCase(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
}

// Kiểm tra từ có nằm trong danh sách từ không ý nghĩa không
int isStopWord(char *word) {
    for (int i = 0; i < stopWordCount; i++) {
        if (strcmp(word, stopWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Kiểm tra từ có phải danh từ riêng không
int isProperNoun(char *word, char prevChar) {
    return isupper(word[0]) && prevChar != '.';
}

// Thêm từ vào bảng chỉ mục
void addWordToIndex(char *word, int line) {
    for (int i = 0; i < indexCount; i++) {
        if (strcmp(indexTable[i].word, word) == 0) {
            indexTable[i].count++;
            char lineStr[10];
            sprintf(lineStr, ",%d", line);
            strcat(indexTable[i].lines, lineStr);
            return;
        }
    }
    // Nếu từ chưa tồn tại trong bảng chỉ mục
    strcpy(indexTable[indexCount].word, word);
    indexTable[indexCount].count = 1;
    sprintf(indexTable[indexCount].lines, "%d", line);
    indexCount++;
}

// Sắp xếp bảng chỉ mục theo thứ tự từ điển
void sortIndexTable() {
    for (int i = 0; i < indexCount - 1; i++) {
        for (int j = i + 1; j < indexCount; j++) {
            if (strcmp(indexTable[i].word, indexTable[j].word) > 0) {
                IndexEntry temp = indexTable[i];
                indexTable[i] = indexTable[j];
                indexTable[j] = temp;
            }
        }
    }
}

int main() {
    FILE *textFile = fopen("vanban.txt", "r");
    FILE *stopWordsFile = fopen("stopw.txt", "r");
    if (!textFile || !stopWordsFile) {
        printf("Error opening files.\n");
        return 1;
    }

    // Đọc stop words
    while (fscanf(stopWordsFile, "%s", stopWords[stopWordCount]) != EOF) {
        toLowerCase(stopWords[stopWordCount]);
        stopWordCount++;
    }
    fclose(stopWordsFile);

    // Đọc văn bản
    char line[MAX_LINE_LENGTH];
    int lineNumber = 0;
    while (fgets(line, MAX_LINE_LENGTH, textFile)) {
        lineNumber++;
        char *token = strtok(line, " ,.!?\r\n");
        char prevChar = '.';
        while (token) {
            char cleanWord[50];
            strcpy(cleanWord, token);
            toLowerCase(cleanWord);
            if (!isStopWord(cleanWord) && !isProperNoun(token, prevChar)) {
                addWordToIndex(cleanWord, lineNumber);
            }
            prevChar = token[strlen(token) - 1];
            token = strtok(NULL, " ,.!?\r\n");
        }
    }
    fclose(textFile);

    // Sắp xếp và in kết quả
    sortIndexTable();
    for (int i = 0; i < indexCount; i++) {
        printf("%s %d,%s\n", indexTable[i].word, indexTable[i].count, indexTable[i].lines);
    }

    return 0;
}
