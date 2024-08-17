#include <bitset>
using namespace std;
bitset<4> binary(string("1111"));
int row[] = {23, 22 ,21, 19, 18};
int col[] = {5, 17, 16, 4};
byte bit;
int rows = 5;
int columns = 4;
unsigned char M[] = {
 B00001001,
 B00001111,
 B00001001,
 B00001001,
 B00001001
};

unsigned char Y[] = {
 B00001001,
 B00000110,
 B00000110,
 B00000110,
 B00000110
};

unsigned char T[] = {
 B00001111,
 B00000110,
 B00000110,
 B00000110,
 B00000110
};

unsigned char L[] = {
 B00001000,
 B00001000,
 B00001000,
 B00001000,
 B00001111
};

unsigned char A[] = {
 B00000110,
 B00001001,
 B00001111,
 B00001001,
 B00001001
};

unsigned char E[] = {
 B00001111,
 B00001000,
 B00001110,
 B00001000,
 B00001111
};

unsigned char R[] = {
 B00001110,
 B00001001,
 B00001110,
 B00001001,
 B00001001
};

unsigned char B[] = {
 B00001110,
 B00001001,
 B00001110,
 B00001001,
 B00001110
};

unsigned char K[] = {
 B00001001,
 B00001010,
 B00001100,
 B00001010,
 B00001001
};

unsigned char O[] = {
 B00001111,
 B00001001,
 B00001001,
 B00001001,
 B00001111
};

unsigned char D[] = {
 B00001110,
 B00001001,
 B00001001,
 B00001001,
 B00001110
};

unsigned char V[] = {
 B00001001,
 B00001001,
 B00001001,
 B00001001,
 B00000110
};


unsigned char U[] = {
 B00001001,
 B00001001,
 B00001001,
 B00001001,
 B00001111
};

unsigned char I[] = {
 B00001111,
 B00000110,
 B00000110,
 B00000110,
 B00001111
};
unsigned char X[] = {
 B00001001,
 B00000110,
 B00000110,
 B00000110,
 B00001001
};

unsigned char N[] = {
 B00001001,
 B00001101,
 B00001011,
 B00001011,
 B00001001
};

void setup() {
   
  for( int i = 0; i <5; i++){
   pinMode(row[i], OUTPUT);
  }
  for( int i = 0; i <4; i++){
   pinMode(col[i], OUTPUT);
  }
  allOff();
}

void loop() {  
  displayLetter(B, 250);
  displayLetter(A, 250);
  displayLetter(L, 250);
  displayLetter(I, 250);
  displayLetter(K, 250);
  allOff();
  delay(120);
  displayLetter(K, 250);
  displayLetter(A, 250);
  displayLetter(N, 250);
  displayLetter(A, 250);
  allOff();
  delay(120);
}

void allOff()
{
  for (int i=0;i<5;i++)
  {
    digitalWrite(row[i], HIGH);
   }
  for (int i=0;i<4;i++)
  {
    digitalWrite(col[i], LOW);
  }
}

void displayLetter(unsigned char* letter, int Delay){
  for (int i = 0; i < (Delay / 1); i++)
  {
    showPattern(letter);
    delay(1);
  }
}

void showPattern(unsigned char* letter)
{
  for (int thisrow=0;thisrow<rows;thisrow++)
  {
    //turn everything off
    allOff();
    //turn on current row
    digitalWrite(row[thisrow], LOW);
    // loop through columns of this row and light
    for (int thiscol=0;thiscol<columns;thiscol++)
    {
      if (bitRead(letter[thisrow],columns-thiscol-1)==1)
      {
        digitalWrite(col[thiscol], HIGH);
      }
      else
      {
        digitalWrite(col[thiscol], LOW);
      }
    }
    delay(1);
  }
}