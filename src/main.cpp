// ########################################
// Nbody-simulation
// - Theme : Cold Collapse
// ########################################

// ----------------------------------------
// Install Packages
// ----------------------------------------
#include <cstdio>     // standard library
#include <cmath>      // math library
#include <iostream>   // standard I/O library of C++
#include <fstream>    // standard file I/O library of C++
#include <time.h>     // For using 'clock()' function
#include <unistd.h>   // For using 'getopt()' function
#include <string.h>   // For using 'string type'
#include <algorithm>  // vector のソート用
#include <functional> // vector のソート用
#include <sys/stat.h> // For get state of directory(For using 'chmod' and so on.)
#include <random>     // For using 'random()' function

// ----------------------------------------
// Define units
// ----------------------------------------
#define G 1.0
#define M 1.0
#define R 1.0

// ----------------------------------------
// Define global valiables ※本当は使わない方が良い
// ----------------------------------------
unsigned int NPtcl = 1024;        // 粒子数
double SoftLen   = (1.0 / 32.0);  // Softening Length
double DT        = (1.0 / 128.0); // Time Step
double DTDisp    = (1.0 / 8.0);   // 画面出力間隔
double DTSnap    = (1.0 / 4.0);   // ファイル出力間隔
double TEnd      = 5.0;           // 終了時間
double RVir      = 0.1;           // ビリアル比
bool BinaryFlg   = false;         // バイナリ出力有無
char DirName[50] = "result/";     // 出力先ディレクトリ名

// ----------------------------------------
// Define functions
// ----------------------------------------
// =========================
// 実行オプションの反映
// =========================
void optionHandling ( int argc, char * argv[] ) {
  // --------------------
  // ** 変数定義
  int optionName;
  extern int optind; // 魔法のフレーズ，気にしない
  // --------------------
  // ** 実行オプションの内容を確認する
  std::cout << "== Change global valiables ==" << std::endl;
  while ( ( optionName = getopt(argc, argv, "o:t:T:n:s:v:d:D:b") ) != -1 ) {
    switch ( optionName ) {
      case 'o':
        strcpy(DirName, optarg);
        std::cout << " * Output directory = " << DirName << std::endl;
        break;
      case 't':
        DT = atof(optarg); // String型をdoubleにする
        std::cout << " * TimeStep = " << DT << std::endl;
        break;
      case 'T':
        TEnd = atof(optarg); // String型をdoubleにする
        std::cout << " * Time to end = " << TEnd << std::endl;
        break;
      case 'n':
        NPtcl = atoi(optarg); // String型をintにする
        std::cout << " * Number of Particles = " << NPtcl << std::endl;
        break;
      case 's':
        SoftLen = atof(optarg); // String型をdoubleにする
        std::cout << " * Softening length = " << SoftLen << std::endl;
        break;
      case 'v':
        RVir = atof(optarg); // String型をdoubleにする
        std::cout << " * Virial ratio = " << RVir << std::endl;
        break;
      case 'd':
        DTSnap = atof(optarg); // String型をdoubleにする
        std::cout << " * Time Step of Fileoutput = " << DTSnap << std::endl;
        break;
      case 'D':
        DTDisp = atof(optarg); // String型をdoubleにする
        std::cout << " * Time Step of Display = " << DTDisp << std::endl;
        break;
      case 'b':
        BinaryFlg = true;
        std::cout << " * Binary output is true. " << std::endl;
        break;
      default:
        std::cerr << "No such option! Available options are here." << std::endl;
        exit(1);
    }
  }
  std::cout << "=============================" << std::endl;
}
// =========================
// ディレクトリ作成
// =========================
void makeDirectory ( char * Name ) {
  struct stat st;
  if ( stat(Name, &st) != 0 ) {
    int ret = -1;
    // Parmisson change (chomod 0777)
    ret = mkdir(Name, 0777);
    if ( ret == 0 ) {
        std::cerr << "Directory... " << Name << " is successfully made." << std::endl;
    } else {
      std::cerr << "Directory... " << Name << " fails to be made." << std::endl;
    }
  }
}
// =========================
// ポテンシャルエネルギー計算
// =========================
double calcPotEne ( double m[], double x[][3] ) {
  double potEne = 0.0;
  double xij[3];
  double r2ij = 0.0;
  for ( int i = 0; i < NPtcl - 1; ++i ) {
    for ( int j = i + 1; j < NPtcl; ++j ) {
      xij[0] = x[j][0] - x[i][0];
      xij[1] = x[j][1] - x[i][1];
      xij[2] = x[j][2] - x[i][2];
      r2ij = (xij[0] * xij[0]) + (xij[1] * xij[1]) + (xij[2] * xij[2]);
      potEne += - (m[i] * m[j]) / sqrt(r2ij + (SoftLen * SoftLen));
    }
  }
  return potEne;
}
// =========================
// 運動エネルギー計算
// =========================
double calcKinEne ( double m[], double v[][3] ) {
  double kinEne = 0;
  for ( int i = 0; i < NPtcl; ++i ) {
    kinEne += 0.5 * m[i]
              * ((v[i][0] * v[i][0]) + (v[i][1] * v[i][1]) + (v[i][2] * v[i][2]));
  }
  return kinEne;
}
// =========================
// ガウシアン関数
// =========================
double gaussian ( void ) {
  // --------------------
  // ** 変数
  double x, y, z, r2;
  // --------------------
  // Box-Mullar
  do {
    x = 2.0 * drand48() - 1.0;
    y = 2.0 * drand48() - 1.0;
    r2 = (x * x) + (y * y);
  } while ( r2 >= 1.0 || r2 == 0.0 );
  z = sqrt( -2.0 * log(r2) / r2 ) * x;
  return z;
}
// =========================
// 初期分布作成
// =========================
void makePtclDistribution ( double m[], double x[][3], double v[][3] ) {
  // --------------------
  // ** 粒子の質量を設定
  for ( int i = 0; i < NPtcl; ++i ) {
    m[i] = (double)M / NPtcl;
  }
  // --------------------
  // ** 粒子の位置を設定
  double r2 = 0.0;
  for ( int i = 0; i < NPtcl; ++i ) {
    do {
      r2 = 0.0;
      for ( int k = 0; k < 3; ++k ) {
        x[i][k] = 2.0 * drand48() - 1.0;
      }
      r2 = (x[i][0] * x[i][0]) + (x[i][1] * x[i][1]) + (x[i][2] * x[i][2]);
    } while ( sqrt(r2) > 1.0 );
  }
  // --------------------
  // ** ポテンシャル計算と速度分散の計算
  double potEne = calcPotEne(m, x);
  double sigma  = sqrt( (2.0 * RVir * fabs(potEne)) / (3.0 * M) );
  // --------------------
  // ** 速度分散からマクスウェル分布で速度を与える
  for ( int i = 0; i < NPtcl; ++i ) {
    for ( int k = 0; k < 3; ++k ) {
      v[i][k] = sigma * gaussian();
    }
  }
}
// =========================
// 重力計算
// =========================
void calcForce ( double m[], double x[][3], double a[][3] ) {
  // --------------------
  // ** 加速度の初期化
  for ( int i = 0; i < NPtcl; ++i ) {
    a[i][0] = 0.0;
    a[i][1] = 0.0;
    a[i][2] = 0.0;
  }
  // --------------------
  // ** 相対位置の計算用変数
  double xij[3];
  double r3inv = 0.0;
  // --------------------
  // ** 重力相互作用を計算する（直接計算法）
  for ( int i = 0; i < NPtcl - 1; ++i ) {
    for ( int j = i + 1; j < NPtcl; ++j ) {
      // --------------------
      // ** 相対距離の計算
      xij[0] = x[j][0] - x[i][0];
      xij[1] = x[j][1] - x[i][1];
      xij[2] = x[j][2] - x[i][2];
      // --------------------
      // ** 粒子間距離の3乗の逆数を計算
      r3inv = 1.0 / pow(sqrt((xij[0] * xij[0]) + (xij[1] * xij[1]) + (xij[2] * xij[2]) + (SoftLen * SoftLen)), 3.0);
      // --------------------
      // ** j粒子からi粒子への作用計算
      a[i][0] += G * m[j] * xij[0] * r3inv;
      a[i][1] += G * m[j] * xij[1] * r3inv;
      a[i][2] += G * m[j] * xij[2] * r3inv;
      // --------------------
      // ** i粒子からj粒子への反作用計算
      a[j][0] += -G * m[i] * xij[0] * r3inv;
      a[j][1] += -G * m[i] * xij[1] * r3inv;
      a[j][2] += -G * m[i] * xij[2] * r3inv;
    }
  }
}
// =========================
// 速度を半ステップ進める
// =========================
void kickTime ( double a[][3], double v[][3] ) {
  for ( int i = 0; i < NPtcl; ++i ) {
    v[i][0] += 0.5 * a[i][0] * DT;
    v[i][1] += 0.5 * a[i][1] * DT;
    v[i][2] += 0.5 * a[i][2] * DT;
  }
}
// =========================
// 位置を１ステップ進める
// =========================
void driftTime ( double x[][3], double v[][3] ) {
  for ( int i = 0; i < NPtcl; ++i ) {
    x[i][0] += v[i][0] * DT;
    x[i][1] += v[i][1] * DT;
    x[i][2] += v[i][2] * DT;
  }
}
// =========================
// 粒子情報を出力する
// =========================
void outputPtclInfo ( double m[], double x[][3], double v[][3], int ID ) {
  // --------------------
  // ** 変数定義
  char filePath[50];
  sprintf(filePath, "%s/%d.dat", DirName, ID);
  std::ofstream ofsPtcl; // Energyに関する情報出力
  // --------------------
  // ** Binary出力/Ascii出力
  if ( BinaryFlg ) {
    ofsPtcl.open(filePath, std::ios::binary);
    // --------------------
    // ** 書き込み用 struct
    struct ptcl {
      float mass;
      float pos[3];
      float vel[3];
      int   idx;
    };
    // --------------------
    // ** 書き込み用のstructを作る
    struct ptcl tmp;
    // --------------------
    // ** 全粒子の情報出力
    for ( int i = 0; i < NPtcl; ++i ) {
      // --------------------
      // ** 書き込み用 struct に情報をコピー
      tmp.mass   = m[i];
      tmp.pos[0] = x[i][0];
      tmp.pos[1] = x[i][1];
      tmp.pos[2] = x[i][2];
      tmp.vel[0] = v[i][0];
      tmp.vel[1] = v[i][1];
      tmp.vel[2] = v[i][2];
      tmp.idx    = i;
      // ----------------------------------------
      // ** 粒子情報の書き込み
      ofsPtcl.write((char *)&tmp, sizeof(ptcl));
    }
  } else {
    // --------------------
    // ** 変数定義
    ofsPtcl.open(filePath);
    // --------------------
    // ** 全粒子の情報出力
    for ( int i = 0; i < NPtcl; ++i ) {
      ofsPtcl << x[i][0] << ","
              << x[i][1] << ","
              << x[i][2] << ","
              << v[i][0] << ","
              << v[i][1] << ","
              << v[i][2] << std::endl;
    }
  }
  // --------------------
  // ** ファイルを閉じる
  ofsPtcl.close();
}
// ----------------------------------------
// Define Main function
// ----------------------------------------
int main ( int argc, char * argv[] ) {
  // --------------------
  // ** 実行オプションがある場合に初期設定を行う
  if ( argc != 1 ) {
    optionHandling(argc, argv);
  }
  // --------------------
  // ** 結果保存用のディレクトリを作る
  makeDirectory(DirName);
  // --------------------
  // ** 粒子の変数定義
  double m[NPtcl]; // 質量
  double x[NPtcl][3], v[NPtcl][3], a[NPtcl][3]; // 位置，速度，加速度
  double vhalf[NPtcl][3]; // Leap-Frog法用の半ステップ進んだ速度
  // --------------------
  // ** 系の変数定義
  double tSys, tSnap, tDisp = 0.0; // 系の経過時間，スナップ時間，画面表示時間
  double pot, kin, eTot = 0.0; // ポテンシャルエネルギー，運動エネルギー，全エネルギー
  double eInit = 0.0; // 初期エネルギー
  int    nStep = 0;   // ステップ数
  // --------------------
  // ** ファイル出力用変数定義 ofstream
  char enePath[50];
  sprintf(enePath, "%s/t-de.dat", DirName);
  std::ofstream ofsEne; // Energyに関する情報出力
  // --------------------
  // ** 初期分布を作る
  srand(1223L);
  makePtclDistribution(m, x, v);
  // --------------------
  // ** 初期エネルギーの計算
  pot = calcPotEne(m, x);
  kin = calcKinEne(m, v);
  eTot = eInit = pot + kin;
  // --------------------
  // ** 初期加速度の計算
  calcForce(m, x, a);
  // --------------------
  // ** 時間積分前の初期状態アウトプット
  outputPtclInfo(m, x, v, nStep);
  // --------------------
  // ** エネルギー情報の出力
  ofsEne.open(enePath); // ファイルを開けっ放しにする
  // --------------------
  // ** 時間積分の開始
  while ( tSys < TEnd ) {
    // --------------------
    // ** 粒子情報スナップ
    if ( (tSys >= tSnap) || ( (tSys + DT) - tSnap ) > (tSnap - tSys) ) {
      // --------------------
      // ** エネルギーと粒子状態の出力
      ofsEne << tSys << "," << pot << "," << kin << "," << (eTot - eInit) / eInit <<  std::endl;
      outputPtclInfo(m, x, v, nStep);
      tSnap += DTSnap;
    }
    // --------------------
    // ** エネルギー計算
    pot = calcPotEne(m, x);
    kin = calcKinEne(m, v);
    eTot = pot + kin;
    // --------------------
    // ** 画面出力
    if ( (tSys >= tDisp) || ( (tSys + DT) - tDisp ) > (tDisp - tSys) ) {
      std::cout << "time: " << tSys << "  energy error: " << (eTot - eInit) / eInit << std::endl;
      tDisp += DTDisp;
    }
    // --------------------
    // ** Leap-Frog
    kickTime(a, v);
    tSys += DT;
    ++nStep;
    driftTime(x, v);
    // --------------------
    // ** 重力計算
    calcForce(m, x, a);
    // --------------------
    // ** Leap-Frog
    kickTime(a, v);
  }
  // --------------------
  // ** エネルギー出力
  ofsEne.close(); // ファイルを閉じる
  return 0;
}
