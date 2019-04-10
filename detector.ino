#define DUGUM_SAYISI 3
#define SATIR_SAYISI DUGUM_SAYISI
#define SUTUN_SAYISI DUGUM_SAYISI + 1   // zaman degiskeni geldi
#define SES_HIZI2 0.001177862   // cm/mikrosaniye
#define TIMEOUT 300000  //ms
#define VOLTAGE_SENSIVITY 3

long int deger0, deger1, deger2, checkpoint, checkpoint2, zaman1, zaman2, zaman3, ilk_zaman = 0, son_zaman = 0;
long double mikrofonlar[DUGUM_SAYISI][2] = {{-15, 0}, {15, 0}, {0, 30}}, 
    uzakliklar[DUGUM_SAYISI], 
    denklemler[SATIR_SAYISI][SUTUN_SAYISI],
    bilinmeyenler[3];
 
int sinyal_geldi0 = 0, sinyal_geldi1 = 0, sinyal_geldi2 = 0, 
    sonraki, yeni_sinyal = 0, ilk_sinyal = -1, baslangic = 0, 
    ortalama_bulundu = 0, sifir_bulundu = 0, 
    j, k, l, m,
    toplam0 = 0, toplam1 = 0, toplam2 = 0;

double katsayi, x[SUTUN_SAYISI];

void setup () 
{
  Serial.begin (9600);
}

void loop () 
{ 
  if((sinyal_geldi0 == 0) && (sinyal_geldi1 == 0) && (sinyal_geldi2 == 0))
  {
    ilk_zaman = micros();
  }

  deger0 = analogRead(A0);
  deger1 = analogRead(A7);
  deger2 = analogRead(A15);  

//  Serial.print("Deger 0: "); Serial.println(deger0);
//  Serial.print("Deger 1: "); Serial.println(deger1);
//  Serial.print("Deger 2: "); Serial.println(deger2);

  /* Ortamin ortalama ses esik degerini hesapla */
  if(baslangic < 50)
  {
    toplam0 += deger0;
    toplam1 += deger1;
    toplam2 += deger2;
    baslangic += 1;
  }
  
  else
  {
    if(ortalama_bulundu == 0)
    {
      toplam0 = toplam0 / 50;
      toplam1 = toplam1 / 50;
      toplam2 = toplam2 / 50;
      ortalama_bulundu = 1;
      Serial.print("Ortalama 0: "); Serial.println(toplam0);
      Serial.print("Ortalama 1: "); Serial.println(toplam1);
      Serial.print("Ortalama 2: "); Serial.println(toplam2);
    }

    /* Mikrofondan gelen sinyalleri al */
    if(((deger0 < (toplam0 - VOLTAGE_SENSIVITY)) || (deger0 > (toplam0 + VOLTAGE_SENSIVITY))) && (sinyal_geldi0 == 0))
    {
      Serial.print("Deger 0: "); Serial.println((double)deger0, 2);

      sinyal_geldi0 = 1;
      if((sinyal_geldi1 == 0) && (sinyal_geldi2 == 0))
      {
        ilk_sinyal = 0;
        son_zaman = ilk_zaman;
      }
      
      uzakliklar[0] = micros() - son_zaman;  
      
      //Serial.print("1. mikrofonun sinyal zamani: "); Serial.println((double)uzakliklar[0], 2);
    }

    if(((deger1 < (toplam1 - VOLTAGE_SENSIVITY)) || (deger1 > (toplam1 + VOLTAGE_SENSIVITY))) && (sinyal_geldi1 == 0))
    {
      Serial.print("Deger 1: "); Serial.println((double)deger1, 2);

      sinyal_geldi1 = 1;
      if((sinyal_geldi0 == 0) && (sinyal_geldi2 == 0))
      {
        ilk_sinyal = 1;
        son_zaman = ilk_zaman;
      }        
      uzakliklar[1] = micros() - son_zaman;
      //Serial.print("2. mikrofonun sinyal zamani: "); Serial.println((double)uzakliklar[1], 2);
    }

    if(((deger2 < (toplam2 - VOLTAGE_SENSIVITY)) || (deger2 > (toplam2 + VOLTAGE_SENSIVITY))) && (sinyal_geldi2 == 0))
    {
      Serial.print("Deger 2: "); Serial.println((double)deger2, 2);

      sinyal_geldi2 = 1;
      if((sinyal_geldi0 == 0) && (sinyal_geldi1 == 0))
      {
        ilk_sinyal = 2;
        son_zaman = ilk_zaman;
      }
      
      uzakliklar[2] = micros() - son_zaman;
      //Serial.print("3. mikrofonun sinyal zamani: "); Serial.println((double)uzakliklar[2], 2);
    }

    /* Denklemi hesapla */
    if((sinyal_geldi0 != 0) && (sinyal_geldi1 != 0) && (sinyal_geldi2 != 0))
    {
      //Serial.print("1. mikrofon"); Serial.println(uzakliklar[0]);
      //Serial.print("2. mikrofon"); Serial.println(uzakliklar[1]);
      //Serial.print("3. mikrofon"); Serial.println(uzakliklar[2]);

      for(int i = 0; i < SATIR_SAYISI; i++){
        if(i == (SATIR_SAYISI - 1)) sonraki = 0;
        else sonraki = i + 1;
        denklemler[i][0] = ((-2)*mikrofonlar[i][0] + 2*mikrofonlar[sonraki][0]);   // X degeri
        denklemler[i][1] = ((-2)*mikrofonlar[i][1] + 2*mikrofonlar[sonraki][1]);   // Y degeri
        denklemler[i][2] = (SES_HIZI2) * (2) * uzakliklar[i];    // t0 degeri
        denklemler[i][3] = ( (SES_HIZI2) * ( (uzakliklar[i] * uzakliklar[i]) - (uzakliklar[sonraki] * uzakliklar[sonraki]) ) - ( (mikrofonlar[i][0] * mikrofonlar[i][0]) - (mikrofonlar[sonraki][0] * mikrofonlar[sonraki][0])) - ((mikrofonlar[i][1] * mikrofonlar[i][1]) - (mikrofonlar[sonraki][1] * mikrofonlar[sonraki][1])));    // sabit degerler,
      }


      for(j = 0; j < SATIR_SAYISI; j++){                  //ilk kosegenden basla
        if(denklemler[j][j] != 1){                                //eger kosegeni 1 degilse
          if(denklemler[j][j] == 0){                              //eger kosegeni 0 ise
            for(l = j; l < SATIR_SAYISI; l++){            //ayni sutunda 0 olmayan degeri bul
              if(denklemler[l][j] != 0){                          //0 olmayan degeri bulursan bunu isaretle
                sifir_bulundu = 1;
                for(k = 0; k < SUTUN_SAYISI; k++){        //0 olmayan satirla 0 olan satirin yerini degistir
                  x[k] = denklemler[j][k];
                  denklemler[j][k] = denklemler[l][k];
                  denklemler[l][k] = x[k];
                }
              } 
              if(sifir_bulundu == 1)  l = SATIR_SAYISI-1; //sifir bulunduysa donguden cikmasini sagla
              else if(sifir_bulundu == 0){
                Serial.print(l);
                Serial.println(". degisken icin tum sutun degerleri sifir, deger belirlenemedi.");
              }  
            }
          }
          
          else{                                           //kosegeni 1 yapmak icin butun elemanları kosegenin degerine bol
            katsayi = denklemler[j][j];
            for(k = j; k < SUTUN_SAYISI; k++){
              if(denklemler[j][k] != 0){                          //sutun degeri 0 ise bolme
                denklemler[j][k] = denklemler[j][k] / katsayi;
              }
            }
          }

        }

    // ALT UCGENI SIFIRLA
        if(j != SATIR_SAYISI - 1){                          
          for(k = j + 1; k < SATIR_SAYISI; k++){            
            if(denklemler[k][j] != 0){
            katsayi = denklemler[k][j];
              for(l = j; l < SUTUN_SAYISI; l++){            //kosegen satirinin tum degerlerini katsayiyla carpip sifirlanacak
                denklemler[k][l] -= denklemler[j][l] * katsayi;             //satir degerlerinden cikart
              }
            }
          }
        }

    //UST UCGEN ERITILIYOR
        else{
      
          for(k = j; k >= 0; k--){
            for(m = k - 1; m >= 0; m--){
              if(denklemler[m][k] != 0){
                katsayi = denklemler[m][k];
                for(l = k; l < SUTUN_SAYISI; l++){
                  denklemler[m][l] -= denklemler[k][l] * katsayi;
                }
              }
            }
          }
            
        }

      }

    bilinmeyenler[0] = denklemler[0][SUTUN_SAYISI - 1];
    bilinmeyenler[1] = denklemler[1][SUTUN_SAYISI - 1];
    bilinmeyenler[2] = denklemler[2][SUTUN_SAYISI - 1];

    Serial.print("X degeri: "); Serial.println((double)bilinmeyenler[0], 2);
    Serial.print("Y degeri: "); Serial.println((double)bilinmeyenler[1], 2);
    Serial.println("-----------------------------------------------------");
    sinyal_geldi0 = 0;
    sinyal_geldi1 = 0;
    sinyal_geldi2 = 0; 
    sifir_bulundu = 0;
    son_zaman = 0;
    delay(100);
    }

    /* Ses tek mikrofona ulasmis ise alicilari sifirla */
    if(son_zaman != 0)
    {
      checkpoint = micros() - son_zaman;
      if((ilk_sinyal == 0) && (abs(uzakliklar[0] - checkpoint) > TIMEOUT))
      {
        ilk_sinyal = -1;
        sinyal_geldi0 = 0;
        sinyal_geldi1 = 0;
        sinyal_geldi2 = 0;
        son_zaman = 0;
        //Serial.print("Checkpoint - uzaklik0: "); Serial.println((double)abs(uzakliklar[0] - checkpoint), 2);
        Serial.println("1. mikrofon sifirlandi");
      }
      else if((ilk_sinyal == 1) && (abs(uzakliklar[1] - checkpoint) > TIMEOUT))
      {
        ilk_sinyal = -1;
        sinyal_geldi0 = 0;
        sinyal_geldi1 = 0;
        sinyal_geldi2 = 0;
        son_zaman = 0;
        //Serial.print("Checkpoint - uzaklik1: "); Serial.println((double)abs(uzakliklar[1] - checkpoint), 2);
        Serial.println("2. mikrofon sifirlandi");
      }
      else if((ilk_sinyal == 2) && (abs(uzakliklar[2] - checkpoint) > TIMEOUT))
      {
        ilk_sinyal = -1;
        sinyal_geldi0 = 0;
        sinyal_geldi1 = 0;
        sinyal_geldi2 = 0;
        son_zaman = 0;
        //Serial.print("Checkpoint - uzaklik2: "); Serial.println((double)abs(uzakliklar[2] - checkpoint), 2);
        Serial.println("3. mikrofon sifirlandi");
      }
    }
  }
}
