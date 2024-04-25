int global;
void main()
{
  int local;
  int duplicate;
  global=10;
  duplicate=1;
  global=global/10;
  local=-global + duplicate;
  duplicate=-duplicate;
  print local;
  print global;
  print duplicate;


}