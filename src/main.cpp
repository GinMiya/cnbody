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
#include <sys/stat.h> // For get state of directory(For using 'chmod' and so on.)
#include <random>     // For using 'random()' function

// ----------------------------------------
// Define units
// ----------------------------------------
#define G (1.0)
#define M (1.0)
#define R (1.0)

// ----------------------------------------
// Define struct
// ----------------------------------------
struct Param {
  unsigned int nPtcl; // 粒子数
  double softLen;     // Softening Length
  double dT;          // Time Step
  double dTDisp;      // 画面出力間隔
  double dTSnap;      // ファイル出力間隔
  double tEnd;        // 終了時間
  double rVir;        // ビリアル比
  bool binaryFlg;     // バイナリ出力有無
  char dirName[50];   // 出力先ディレクトリ名
  void dispParams ( void ) {
    std::cout << "==== Parameter Setting   ====" << std::endl;
    std::cout << " * Number of Particles = "     << this->nPtcl     << std::endl;
    std::cout << " * Softening length = "        << this->softLen   << std::endl;
    std::cout << " * TimeStep = "                << this->dT        << std::endl;
    std::cout << " * Time Step of Display = "    << this->dTDisp    << std::endl;
    std::cout << " * Time Step of Fileoutput = " << this->dTSnap    << std::endl;
    std::cout << " * Finish Time = "             << this->tEnd      << std::endl;
    std::cout << " * Virial ratio = "            << this->rVir      << std::endl;
    std::cout << " * Binary output = "           << this->binaryFlg << std::endl;
    std::cout << " * Output directory = "        << this->dirName   << std::endl;
    std::cout << "=============================" << std::endl;
  }
};

// ----------------------------------------
// Define functions
// ----------------------------------------
// =========================
// 実行オプションの反映
// =========================
void optionHandling ( int argc, char * argv[], Param * ParamSet ) {
  // --------------------
  // ** 変数定義
  int optionName;
  extern int optind;
  // --------------------
  // ** 実行オプションの内容を確認する
  std::cout << "===== Change Parameters =====" << std::endl;
  while ( ( optionName = getopt(argc, argv, "o:t:T:n:s:v:d:D:b") ) != -1 ) {
    switch ( optionName ) {
      case 'o':
        strcpy(ParamSet->dirName, optarg);
        std::cout << " * Output directory = " << ParamSet->dirName << std::endl;
        break;
      case 't':
        ParamSet->dT = atof(optarg); // String型をdoubleにする
        std::cout << " * TimeStep = " << ParamSet->dT << std::endl;
        break;
      case 'T':
        ParamSet->tEnd = atof(optarg); // String型をdoubleにする
        std::cout << " * Finish Time = " << ParamSet->tEnd << std::endl;
        break;
      case 'n':
        ParamSet->nPtcl = atoi(optarg); // String型をintにする
        std::cout << " * Number of Particles = " << ParamSet->nPtcl << std::endl;
        break;
      case 's':
        ParamSet->softLen = atof(optarg); // String型をdoubleにする
        std::cout << " * Softening length = " << ParamSet->softLen << std::endl;
        break;
      case 'v':
        ParamSet->rVir = atof(optarg); // String型をdoubleにする
        std::cout << " * Virial ratio = " << ParamSet->rVir << std::endl;
        break;
      case 'd':
        ParamSet->dTSnap = atof(optarg); // String型をdoubleにする
        std::cout << " * Time Step of Fileoutput = " << ParamSet->dTSnap << std::endl;
        break;
      case 'D':
        ParamSet->dTDisp = atof(optarg); // String型をdoubleにする
        std::cout << " * Time Step of Display = " << ParamSet->dTDisp << std::endl;
        break;
      case 'b':
        ParamSet->binaryFlg = true;
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
double calcPotEne ( double m[], double x[][3], Param * ParamSet ) {
  double potEne = 0.0;
  double xij[3];
  double r2ij = 0.0;
  for ( int i = 0; i < ParamSet->nPtcl - 1; ++i ) {
    for ( int j = i + 1; j < ParamSet->nPtcl; ++j ) {
      xij[0] = x[j][0] - x[i][0];
      xij[1] = x[j][1] - x[i][1];
      xij[2] = x[j][2] - x[i][2];
      r2ij = (xij[0] * xij[0]) + (xij[1] * xij[1]) + (xij[2] * xij[2]);
      potEne += - (m[i] * m[j]) / sqrt(r2ij + (ParamSet->softLen * ParamSet->softLen));
    }
  }
  return potEne;
}
// =========================
// 運動エネルギー計算
// =========================
double calcKinEne ( double m[], double v[][3], Param * ParamSet ) {
  double kinEne = 0;
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
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
void makePtclDistribution ( double m[], double x[][3], double v[][3], Param * ParamSet ) {
  // --------------------
  // ** 粒子の質量を設定
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
    m[i] = (double)M / ParamSet->nPtcl;
  }
  // --------------------
  // ** 粒子の位置を設定
  double r2 = 0.0;
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
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
  double potEne = calcPotEne(m, x, ParamSet);
  double sigma  = sqrt( (2.0 * ParamSet->rVir * fabs(potEne)) / (3.0 * M) );
  // --------------------
  // ** 速度分散からマクスウェル分布で速度を与える
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
    for ( int k = 0; k < 3; ++k ) {
      v[i][k] = sigma * gaussian();
    }
  }
}
// =========================
// 重力計算
// =========================
void calcForce ( double m[], double x[][3], double a[][3], Param * ParamSet ) {
  // --------------------
  // ** 加速度の初期化
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
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
  for ( int i = 0; i < ParamSet->nPtcl - 1; ++i ) {
    for ( int j = i + 1; j < ParamSet->nPtcl; ++j ) {
      // --------------------
      // ** 相対距離の計算
      xij[0] = x[j][0] - x[i][0];
      xij[1] = x[j][1] - x[i][1];
      xij[2] = x[j][2] - x[i][2];
      // --------------------
      // ** 粒子間距離の3乗の逆数を計算
      r3inv = 1.0 / pow(sqrt((xij[0] * xij[0]) + (xij[1] * xij[1]) + (xij[2] * xij[2]) + (ParamSet->softLen * ParamSet->softLen)), 3.0);
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
void kickTime ( double a[][3], double v[][3], Param * ParamSet ) {
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
    v[i][0] += 0.5 * a[i][0] * ParamSet->dT;
    v[i][1] += 0.5 * a[i][1] * ParamSet->dT;
    v[i][2] += 0.5 * a[i][2] * ParamSet->dT;
  }
}
// =========================
// 位置を１ステップ進める
// =========================
void driftTime ( double x[][3], double v[][3], Param * ParamSet ) {
  for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
    x[i][0] += v[i][0] * ParamSet->dT;
    x[i][1] += v[i][1] * ParamSet->dT;
    x[i][2] += v[i][2] * ParamSet->dT;
  }
}
// =========================
// 粒子情報を出力する
// =========================
void outputPtclInfo ( double m[], double x[][3], double v[][3], int ID, Param * ParamSet ) {
  // --------------------
  // ** 変数定義
  char filePath[50];
  sprintf(filePath, "%s/%d.dat", ParamSet->dirName, ID);
  std::ofstream ofsPtcl; // Energyに関する情報出力
  // --------------------
  // ** Binary出力/Ascii出力
  if ( ParamSet->binaryFlg ) {
    ofsPtcl.open(filePath, std::ios::binary);
    // --------------------
    // ** 書き込み用 struct
    struct ptcl {
      float pos[3];
      float vel[3];
    };
    // --------------------
    // ** 書き込み用のstructを作る
    struct ptcl tmp;
    // --------------------
    // ** 全粒子の情報出力
    for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
      // --------------------
      // ** 書き込み用 struct に情報をコピー
      tmp.pos[0] = x[i][0];
      tmp.pos[1] = x[i][1];
      tmp.pos[2] = x[i][2];
      tmp.vel[0] = v[i][0];
      tmp.vel[1] = v[i][1];
      tmp.vel[2] = v[i][2];
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
    for ( int i = 0; i < ParamSet->nPtcl; ++i ) {
      ofsPtcl << x[i][0] << " "
              << x[i][1] << " "
              << x[i][2] << " "
              << v[i][0] << " "
              << v[i][1] << " "
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
  // ** シミュレーションの設定パラメータのデフォルト値をセットする　（今回はスタック領域に確保する）
  struct Param ParamSet;
  ParamSet.nPtcl     = 1024;           // 粒子数
  ParamSet.softLen   = 1.0 / 32.0;     // Softening Length
  ParamSet.dT        = 1.0 / 128.0;    // Time Step
  ParamSet.dTDisp    = 1.0 / 8.0;      // 画面出力間隔
  ParamSet.dTSnap    = 1.0 / 4.0;      // ファイル出力間隔
  ParamSet.tEnd      = 5.0;            // 終了時間
  ParamSet.rVir      = 0.1;            // ビリアル比
  ParamSet.binaryFlg = false;          // バイナリ出力有無
  strcpy(ParamSet.dirName, "result/"); // 出力先ディレクトリ名
  // --------------------
  // ** 実行オプションがある場合に設定パラメータの変更を行う
  if ( argc != 1 ) {
    optionHandling(argc, argv, &ParamSet);
  }
  // --------------------
  // ** パラメータ一覧を出力
  ParamSet.dispParams();
  // --------------------
  // ** 粒子の変数定義
  double m[ParamSet.nPtcl]; // 質量
  double x[ParamSet.nPtcl][3], v[ParamSet.nPtcl][3], a[ParamSet.nPtcl][3]; // 位置，速度，加速度
  double vhalf[ParamSet.nPtcl][3]; // Leap-Frog法用の半ステップ進んだ速度
  // --------------------
  // ** 系の変数定義
  double tSys = 0.0, tSnap = 0.0, tDisp = 0.0; // 系の経過時間，スナップ時間，画面表示時間
  double pot = 0.0, kin = 0.0, eTot = 0.0;     // ポテンシャルエネルギー，運動エネルギー，全エネルギー
  double eInit = 0.0; // 初期エネルギー
  int    nStep = 0;   // ステップ数
  // --------------------
  // ** ファイル出力用変数定義 ofstream
  char enePath[50];
  sprintf(enePath, "%s/energy.dat", ParamSet.dirName);
  std::ofstream ofsEne; // Energyに関する情報出力
  // --------------------
  // ** 初期分布を作る
  makePtclDistribution(m, x, v, &ParamSet);
  // --------------------
  // ** 初期エネルギーの計算
  pot = calcPotEne(m, x, &ParamSet);
  kin = calcKinEne(m, v, &ParamSet);
  eTot = eInit = pot + kin;
  // --------------------
  // ** 初期加速度の計算
  calcForce(m, x, a, &ParamSet);
  // --------------------
  // ** 結果保存用のディレクトリを作る
  makeDirectory(ParamSet.dirName);
  // --------------------
  // ** 時間積分前の初期状態アウトプット
  outputPtclInfo(m, x, v, nStep, &ParamSet);
  // --------------------
  // ** エネルギー情報の出力
  ofsEne.open(enePath); // ファイルを開けっ放しにする
  // --------------------
  // ** 時間積分の開始
  while ( tSys < ParamSet.tEnd ) {
    // --------------------
    // ** 粒子情報スナップ
    if ( (tSys >= tSnap) || ( (tSys + ParamSet.dT) - tSnap ) > (tSnap - tSys) ) {
      // --------------------
      // ** エネルギーと粒子状態の出力
      ofsEne << tSys << " " << pot << " " << kin << " " << (eTot - eInit) / eInit <<  std::endl;
      outputPtclInfo(m, x, v, nStep, &ParamSet);
      tSnap += ParamSet.dTSnap;
    }
    // --------------------
    // ** エネルギー計算
    pot = calcPotEne(m, x, &ParamSet);
    kin = calcKinEne(m, v, &ParamSet);
    eTot = pot + kin;
    // --------------------
    // ** 画面出力
    if ( (tSys >= tDisp) || ( (tSys + ParamSet.dT) - tDisp ) > (tDisp - tSys) ) {
      std::cout << "time: " << tSys << "  energy error: " << (eTot - eInit) / eInit << std::endl;
      tDisp += ParamSet.dTDisp;
    }
    // --------------------
    // ** Leap-Frog
    kickTime(a, v, &ParamSet);
    tSys += ParamSet.dT;
    ++nStep;
    driftTime(x, v, &ParamSet);
    // --------------------
    // ** 重力計算
    calcForce(m, x, a, &ParamSet);
    // --------------------
    // ** Leap-Frog
    kickTime(a, v, &ParamSet);
  }
  // --------------------
  // ** エネルギー出力
  ofsEne.close(); // ファイルを閉じる
  return 0;
}
