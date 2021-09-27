#include "header.h"

// mylscpu 명령어의 경우 필수 구현 항목은 
// CPU Vendor ID,
// CPU 모델명, 
// CPU 속도, 
// 캐쉬 크기(L1i, L1d, L2 캐쉬) 
// ﻿이며 그 외의 항목은 추가 option으로 취급함﻿

enum IDXS{
  _ARCHITECTURE_        // 0
  , _CPUMODE_           // 1
  , _BYTEORDER_         // 2
  , _CPUS_              // 3
  , _ONLINECPUS_        // 4
  , _THREAD_PER_CORE_   // 5
  , _CORE_PER_SOCKET_   // 6
  , _SOCKET_CNT_        // 7
  , _NUMA_MODE_         // 8
  , _VENDOR_ID_         // 9
  , _CPU_FAMILY_        // 10
  , _MODEL_CNT_         // 11
  , _MODEL_NAME_        // 12
  , _STEPPING_          // 13
  , _CPU_MHZ_           // 14 
  , _MIPS_              // 15
  , _HYPER_VENDOR_      // 16
  , _VIRTUAL_TYPE_      // 17
  , _L1D_CACHE_         // 18
  , _L1I_CACHE_         // 19
  , _L2_CACHE_          // 20
  , _L3_CACHE_          // 21
  , _CPUS_IN_NUMA_NODE_ // 22
  , _FLAGS_             // 23
};
typedef struct {
  char _ARCHITECTURE     [BUFFER_SIZE];   // 0
  char _CPUMODE          [BUFFER_SIZE];   // 1
  char _BYTEORDER        [BUFFER_SIZE];   // 2
  char _CPUS             [5];             // 3
  char _ONLINECPUS       [100];           // 4
  char _THREAD_PER_CORE  [10];            // 5
  char _CORE_PER_SOCKET  [10];            // 6
  char _SOCKET_CNT       [10];            // 7
  char _NUMA_MODE        [10];            // 8
  char _VENDOR_ID        [BUFFER_SIZE];   // 9
  char _CPU_FAMILY       [10];            // 10
  char _MODEL_CNT        [10];            // 11
  char _MODEL_NAME       [BUFFER_SIZE];   // 12
  char _STEPPING         [10];            // 13
  char _CPU_MHZ          [BUFFER_SIZE];   // 14 
  char _MIPS             [BUFFER_SIZE];   // 15
  char _HYPER_VENDOR     [BUFFER_SIZE];   // 16
  char _VIRTUAL_TYPE     [BUFFER_SIZE];   // 17
  char _L1D_CACHE        [BUFFER_SIZE];   // 18
  char _L1I_CACHE        [BUFFER_SIZE];   // 19
  char _L2_CACHE         [BUFFER_SIZE];   // 20
  char _L3_CACHE         [BUFFER_SIZE];   // 21
  char _CPUS_IN_NUMA_NODE[10];            // 22
  char _FLAGS            [BUFFER_SIZE];   // 23
}MyLsCpu;

MyLsCpu info;

void Update(){
  FILE * fp = fopen(_CPUINFO_PATH_,"r");
  char buff[BUFFER_SIZE];
  memset(buff,'\0',BUFFER_SIZE);

  int cnt = 0;



  /* CPU VENDOR ID 에 관한 정보 Get */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_VENDOR_ID;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }  
  int pos=0;
  while(buff[pos++] != ':');
  char ptr[BUFFER_SIZE];
  sprintf(info._VENDOR_ID,"%s",buff+pos+1);



  /* CPU 모델 CNT */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_MODEL;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._MODEL_CNT,"%s",buff+pos+1);



  /* CPU 모델명 */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_MODEL_NAME;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._MODEL_NAME,"%s",buff+pos+1);


  /* CPU CORES */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_CPU_CORES;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._CPUS,"%s",buff+pos+1);

  /* Flags */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_FLAGS;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._FLAGS,"%s",buff+pos+1);

  /* MHZ */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_CPU_MHZ;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._CPU_MHZ,"%s",buff+pos+1);

  /* L3 Mem */
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<=CPUINFO_CACHE_SIZE;i++){
    memset(buff,'\0',BUFFER_SIZE);
    fgets(buff,BUFFER_SIZE,fp);
  }
  pos = 0;
  while(buff[pos++] != ':');
  memset(ptr,'\0',BUFFER_SIZE);
  sprintf(info._L3_CACHE,"%s",buff+pos+1);


  /* L1i, L1d, L2 메모리에 관한 정보 Get */
  DIR * dir = NULL;
  struct dirent * file = NULL;
  if( (dir  = opendir(_L1_MEM_PATH_) ) == NULL){
    fprintf(stderr,"%s directory 정보를 읽을 수 없습니다.",_L1_MEM_PATH_);
    return ;
  }
  while( (file = readdir(dir)) != NULL){
    // dir 이름이 LMEM_DIR_NAME(index)를 포함할 경우 해당 dir 를 탐색
    if(strstr(file->d_name,L_MEM_DIR_NAME) != NULL){
      // level 검사
      char parent_path[BUFFER_SIZE];
      char temp_path[BUFFER_SIZE];
      sprintf(parent_path,"%s/%s/",_L1_MEM_PATH_,file->d_name);
      sprintf(temp_path,"%s/%s",parent_path,L_MEM_LEVEL);
      /*********************************************/
      // temp_path : ${_L1_MEM_PATH_}/index*/level 
      // parent_path : ${_L1_MEM_PATH_}/index*
      /*********************************************/

      freopen(temp_path,"r",fp);
      char level = '\0';
      if((level = fgetc(fp)) == '1'){
        // L1i Memory 인지 L1d Memory 인지 검사
        char temp2_path[BUFFER_SIZE];
        sprintf(temp2_path,"%s/%s",parent_path,L_MEM_TYPE);
        freopen(temp2_path,"r",fp);
        char type = '\0';
        type = getc(fp);
        if( type == 'D' || type == 'd'){
          // L1d Memory
          // size get
          char temp3_path[BUFFER_SIZE];
          sprintf(temp3_path,"%s/%s",parent_path,L_MEM_SIZE);
          freopen(temp3_path,"r",fp);
          fgets(info._L1D_CACHE,BUFFER_SIZE,fp);
        }
        else if( type == 'i' || type == 'I'){
          // L1d Memory
          // size get
          char temp3_path[BUFFER_SIZE];
          sprintf(temp3_path,"%s/%s",parent_path,L_MEM_SIZE);
          freopen(temp3_path,"r",fp);
          fgets(info._L1I_CACHE,BUFFER_SIZE,fp);
        }
        
      }
      else if(level == '2'){
        // L2 Memory
        char temp2_path[BUFFER_SIZE];
        sprintf(temp2_path,"%s/%s",parent_path,L_MEM_SIZE);
          freopen(temp2_path,"r",fp);
          fgets(info._L2_CACHE,BUFFER_SIZE,fp);
      }
    }
  }

}

void View_mylscpu(){
  // Vendor ID 출력
  fprintf(stdout,"%-21s : %s",VENDOR_ID ,info._VENDOR_ID);
  // Model cnt 출력
  fprintf(stdout,"%-21s : %s",MODEL_CNT ,info._MODEL_CNT);
  // Model name 출력
  fprintf(stdout,"%-21s : %s",MODEL_NAME ,info._MODEL_NAME);
  // CPU CORES 출력
  fprintf(stdout,"%-21s : %s",CPUS ,info._CPUS);
  // CPU MHZ 출력
  fprintf(stdout,"%-21s : %s",CPU_MHZ ,info._CPU_MHZ);
  // L1d Memory 출력
  fprintf(stdout,"%-21s : %s",L1D_CACHE ,info._L1D_CACHE);
  // L1i Memory 출력
  fprintf(stdout,"%-21s : %s",L1I_CACHE ,info._L1I_CACHE);
  // L2 Memory 출력
  fprintf(stdout,"%-21s : %s",L2_CACHE ,info._L2_CACHE);
  // Flags 출력
  fprintf(stdout,"%-21s : %s",FLAGS ,info._FLAGS);
}

int main(int args, char * argv[]){
  Update();
  View_mylscpu();
  return 0;
}