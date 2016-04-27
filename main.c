#include"header.h"

void main()
{
  int STARTPOS, refcm, ref , inp;
  char stop, refstr[2], *ptr;
  double pidOut, error, pValue, iValue, dValue, setservo, filteredInp;
  float dt = 0, t = 0, STARTTID = 0;
  FILE *fpGNU;//Open file for datalogging
  fpGNU = fopen("GNUres.dat", "w");

  CPhidgetInterfaceKitHandle sensor = 0; //Create handels for Phidgets
  CPhidgetServoHandle servo = 0;

  initialize(&STARTPOS, &filteredInp, &sensor, &servo);
  CPhidgetServo_setPosition (servo, 0, 120);
  sleep(3);

  printf("\nEnter wanted position from current position:\n");//Ask user for referance
  scanf("%d",&refcm);

  t = gettime();//Aquire time
  STARTTID = gettime();
  initscr();//Start ncurses mode
  cbreak();// Disable line buffering
  printw("Press q or Q to end positioning, or press p to give new position from origin.\n");//Use printw in ncurses mode

   while (1) {
timeout(0);//Set waiting time for keystrokes to zero
     ref= STARTPOS-refcm*100/27;//Calculate true referance from referance given in centimeters

     CPhidgetInterfaceKit_getSensorValue(sensor, 0, &inp);//Get position from potiometer

     filterInp(&inp, &filteredInp);

     pidOut=pidRegulator(&ref, &t, &error, &pValue, &iValue, &dValue, &filteredInp); //calculate correction in PID controller

     setservo = correctPIDOutput(&pidOut);

     CPhidgetServo_setPosition (servo, 0, setservo);//Set servo

     t = gettime();//Update time

     //printw("error %f\t pidOut:%f\t p_value:%f\t i_value:%f\t d_value:%f\t setservo:%f\r",error, pidOut, pValue, iValue, dValue, setservo);
    // printw("inp:%d\t,ref:%d\t, STARTPOS:%d\t, refcm:%d\r", inp, ref, STARTPOS, refcm);
     //fprintf(fpGNU,"%f %f %f %d %d %f %f %f %f %d\n", t-STARTTID, error, pidOut, inp, ref, pValue, iValue, dValue, setservo, refcm);//Print results to resultsfile
     fprintf(fpGNU,"%f %f %f %d %d %f %f %f %f %d\n", t-STARTTID, error, pidOut, inp, ref, pValue, iValue, dValue, setservo, refcm);//Print results to resultsfile

     stop = getch();//Check if user sends stop signal
     if(stop == 'q'){
            endwin();
            printf("\nStopping\n");
            break;

     }//eo if
          else if(stop == 'p') {
            printf("\nEnter new Pos\n");
            timeout(-1);//wait til user types position
            getstr(refstr);
            refcm=strtol(refstr,&ptr, 10);
          }

    usleep(30000);  //"Wait for 0.1 sec//100000
    }//end of while


    //endwin();//End ncurses
    printf("\nOut of while\n");
  system("gnuplot -p 'GNUcmd.gp'");//Plot results in gnuplot

  closeDelete(sensor, servo, fpGNU);//Close and Delete phidget handles

  }//eofmain
