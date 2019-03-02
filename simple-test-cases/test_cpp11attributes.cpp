
float oven(int stuff, int baked, float cherry)
{
  float cake [[clang::annotate("range -3000 3000")]] = baked + stuff;
  return cake + cherry;
}


