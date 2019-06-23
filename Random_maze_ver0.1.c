#include<stdio.h>
#include<handy.h>
#include<stdlib.h>
#include<time.h>

//プロトタイプ宣言
int SetPillar(int m, int n,int MAP[][m]);
int Opening(void);

int main(){
  int n = 31;
  int m = 21;
  int n_m = n * m;
  int MAP[n][m];  //マップ情報
  int i, j;
  int pillar_num;  //柱の数x

  pillar_num = SetPillar(m,n,MAP);
  printf("%d\n",pillar_num);

  int NO_DUPLICATIONRAND[pillar_num];

  HgOpen(1000,700);

  HgGetChar();
  HgClose();
  return 0;
}

int Opening(void){
  return 0;
}

int SetPillar(int m, int n ,int MAP[][m]){
  int i,j;
  int pillar_num = 0;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      if(i==0 || j==0 || i==n-1 || j== m-1) {
        MAP[i][j] = 10;
      }
      else if(i%2==0 && j%2==0){
        MAP[i][j] = 5;
        pillar_num++;
      }
      else{
        MAP[i][j] = 0;
      }
    }
  }
  return pillar_num;
}
