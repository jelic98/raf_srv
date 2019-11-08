#ifndef H_FILE_INCLUDE
#define H_FILE_INCLUDE

FILE* pxFout;

void vBatchLoad(BatchType_t*);
void vReportOpen();
void vReportAdd(TaskType_t*);
void vReportClose();

#endif
