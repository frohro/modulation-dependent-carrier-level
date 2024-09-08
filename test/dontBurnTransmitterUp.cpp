// This is not used.  According to the manual YSE27 does this after the DCC board is finished with its
// processing.  It just wasn't easy to find in the manual.  This is kept as a legacy at this point in case it is
// needed for something later or I'm convinced I'm wrong by someone.


int16_t dontBurnTransmitterUp(uint16_t input)
{
  static unsigned long startTime = 0;
  static bool return2048 = false;
  if (input > 2500)
  {
    if (startTime == 0)
    {
      startTime = micros();
    }
    else if (micros() - startTime >= 10000)
    { // 10 ms = 10000 us
      return2048 = true;
    }
  }
  else
  {
    startTime = 0;
  }
  if (return2048)
  {
    if (input < 2048)
    {
      return2048 = false;
    }
    else
    {
      return 2048;
    }
  }
  return input;
}
