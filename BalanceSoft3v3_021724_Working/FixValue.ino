
const int Array_Size = 6;
const int Array_Count = 2;


int Value[Array_Count][Array_Size];
int Array_Index[Array_Count];

int RC_Fix(int Channel, int Val_In) {

  /// store the value
  Value[Channel][Array_Index[Channel]] = Val_In;



  ////// increase the array index for the next loop
  if (Array_Index[Channel] < Array_Size) {
    Array_Index[Channel]++;
  }
  else {
    Array_Index[Channel] = 0;
  }


  //// find the highest value in the array
  int HighestVal = 0;

  for (int x = 0; x < Array_Size; x++) {

    if ( HighestVal < Value[Channel][x]) {
      HighestVal = Value[Channel][x];
    }


  }
  return HighestVal;

}






const int Avg_Array_Quantity = 2;
const int Avg_Array_Size = 10;


double Avg_Value[Avg_Array_Quantity][Avg_Array_Size];
int Avg_Array_Index[Avg_Array_Quantity];

double Average1(int Avg_Channel, double Avg_Val_In) {

  Avg_Value[Avg_Channel][Avg_Array_Index[Avg_Channel]] = Avg_Val_In;


  ////// increase the array index for the next loop
  if (Avg_Array_Index[Avg_Channel] < Avg_Array_Size) {
    Avg_Array_Index[Avg_Channel]++;
  }
  else {
    Avg_Array_Index[Avg_Channel] = 0;
    //Serial.println("res");
  }


  //// find the highest value in the array
  double Out_Value = 0;

  for (int x = 0; x < Avg_Array_Size; x++) {
    Out_Value += Avg_Value[Avg_Channel][x];

  }
  //  Serial.print("\t");
  //   Serial.println((Out_Value/(double)Avg_Array_Size )-750.0);
  double Output = Out_Value / (double)Avg_Array_Size;

  return Output;

}

double map2(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
