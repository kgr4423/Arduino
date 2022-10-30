
// include折り畳み
#include <Camera.h>

//about gps
#include <GNSS.h>
#include <GNSSPositionData.h>

//#include <iostream>
//#include <string>       // ヘッダファイルインクルード
using namespace std;         //  名前空間指定

#include "gnss_tracker.h"
#include "gnss_nmea.h"
#include "gnss_file.h"

/* Config file */
#define CONFIG_FILE_NAME    "tracker.ini"  /**< Config file name */
#define CONFIG_FILE_SIZE    4096           /**< Config file size */

/* Index file */
#define INDEX_FILE_NAME    "index.ini"     /**< Index file name */
#define INDEX_FILE_SIZE    16              /**< Index file size */

#define STRING_BUFFER_SIZE  128            /**< %String buffer size */
#define NMEA_BUFFER_SIZE    128            /**< NMEA buffer size */
#define OUTPUT_FILENAME_LEN 16             /**< Output file name length */

/* Default parameter. */
#define DEFAULT_INTERVAL_SEC    1          /**< Default positioning interval in seconds*/
#define DEFAULT_ACTIVE_SEC      60         /**< Default positioning active in seconds */
#define DEFAULT_SEEP_SEC        240        /**< Default positioning sleep in seconds */
#define INITIAL_ACTIVE_TIME     300        /**< Initial positioning active in seconds */
#define IDLE_ACTIVE_TIME        600        /**< Idle positioning active in seconds */

#define SERIAL_BAUDRATE     115200         /**< Serial baud rate */

#define SEPARATOR           0x0A           /**< Separator */

/**
 * @enum LoopState
 * @brief State of loop
 */
enum LoopState {
  eStateSleep,  /**< Loop is not activated */
  eStateActive  /**< Loop is activated */
};

/**
 * @enum TrackerMode
 * @brief Tracker mode
 */
enum TrackerMode {
  eModeNormal = 0,  /**< Run positioning. Output NMEA text. */
  eModeShell        /**< Run nsh to print and delete NMEA text. */
};

/**
 * @enum ParamSat
 * @brief Satellite system
 */
enum ParamSat {
  eSatGps,            /**< GPS */
  eSatGlonass,        /**< GLONASS */
  eSatGpsSbas,        /**< GPS+SBAS */
  eSatGpsGlonass,     /**< GPS+Glonass */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA */
  eSatGpsBeidou,      /**< GPS+BeiDou */
  eSatGpsGalileo,     /**< GPS+Galileo */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA */
  eSatGpsBeidouQz1c,  /**< GPS+BeiDou+QZSS_L1CA */
  eSatGpsGalileoQz1c, /**< GPS+Galileo+QZSS_L1CA */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S */
};

/**
 * @struct ConfigParam
 * @brief Configuration parameters
 */
typedef struct
{
  ParamSat      SatelliteSystem;  /**< Satellite system(GPS/GLONASS/ALL). */
  boolean       NmeaOutUart;      /**< Output NMEA message to UART(TRUE/FALSE). */
  boolean       NmeaOutFile;      /**< Output NMEA message to file(TRUE/FALSE). */
  boolean       BinaryOut;        /**< Output binary data to file(TRUE/FALSE). */
  unsigned long IntervalSec;      /**< Positioning interval sec(1-300). */
  unsigned long ActiveSec;        /**< Positioning active sec(60-300). */
  unsigned long SleepSec;         /**< Positioning sleep sec(0-240). */
  SpPrintLevel  UartDebugMessage; /**< Uart debug message(NONE/ERROR/WARNING/INFO). */
} ConfigParam;

SpGnss Gnss;                            /**< SpGnss object */
ConfigParam Parameter;                  /**< Configuration parameters */
unsigned int Mode;                      /**< Tracker mode */
char FilenameTxt[OUTPUT_FILENAME_LEN];  /**< Output file name */
char FilenameBin[OUTPUT_FILENAME_LEN];  /**< Output binary file name */
AppPrintLevel AppDebugPrintLevel;       /**< Print level */
unsigned long SetActiveSec;             /**< Positioning active sec(60-300) */

/**
 * @brief Compare parameter.
 * 
 * @param [in] Input Parameter to compare
 * @param [in] Refer Reference parameter
 * @return 0 if equal
 */
static int ParamCompare(const char *Input , const char *Refer)
{
  /* Set argument. */
  String InputStr = Input;
  String ReferStr = Refer;

  /* Convert to upper case. */
  InputStr.toUpperCase();
  ReferStr.toUpperCase();

  /* Compare. */
  return memcmp(InputStr.c_str(), ReferStr.c_str(), strlen(ReferStr.c_str()));
}

/**
 * @brief Turn on / off the LED0 for CPU active notification.
 */
static void Led_isActive(void)
{
  static int state = 1;
  if (state == 1)
  {
    ledOn(PIN_LED0);
    state = 0;
  }
  else
  {
    ledOff(PIN_LED0);
    state = 1;
  }
}

/**
 * @brief Turn on / off the LED1 for positioning state notification.
 * 
 * @param [in] state Positioning state
 */
static void Led_isPosfix(bool state)
{
  if (state == 1)
  {
    ledOn(PIN_LED1);
  }
  else
  {
    ledOff(PIN_LED1);
  }
}

/**
 * @brief Turn on / off the LED2 for file SD access notification.
 * 
 * @param [in] state SD access state
 */
static void Led_isSdAccess(bool state)
{
  if (state == 1)
  {
    ledOn(PIN_LED2);
  }
  else
  {
    ledOff(PIN_LED2);
  }
}

/**
 * @brief Turn on / off the LED3 for error notification.
 * 
 * @param [in] state Error state
 */
static void Led_isError(bool state)
{
  if (state == 1)
  {
    ledOn(PIN_LED3);
  }
  else
  {
    ledOff(PIN_LED3);
  }
}

/**
 * @brief Convert configuration parameters to String
 * 
 * @param [in] pConfigParam Configuration parameters
 * @return Configuration parameters as String
 */
static String MakeParameterString(ConfigParam *pConfigParam)
{
  const char *pComment;
  const char *pParam;
  const char *pData;
  char StringBuffer[STRING_BUFFER_SIZE];
  String ParamString;

  /* Set SatelliteSystem. */
  pComment = "; Satellite system(GPS/GLONASS/SBAS/QZSS_L1CA/QZSS_L1S)";
  pParam = "SatelliteSystem=";
  switch (pConfigParam->SatelliteSystem)
  {
    case eSatGps:
      pData = "GPS";
      break;

    case eSatGpsSbas:
      pData = "GPS+SBAS";
      break;

    case eSatGlonass:
      pData = "GLONASS";
      break;

    case eSatGpsGlonass:
      pData = "GPS+GLONASS";
      break;

    case eSatGpsBeidou:
      pData = "GPS+BEIDOU";
      break;

    case eSatGpsGalileo:
      pData = "GPS+GALILEO";
      break;

    case eSatGpsQz1c:
      pData = "GPS+QZSS_L1CA";
      break;

    case eSatGpsQz1cQz1S:
      pData = "GPS+QZSS_L1CA+QZSS_L1S";
      break;

    case eSatGpsBeidouQz1c:
      pData = "GPS+BEIDOU+QZSS_L1CA";
      break;

    case eSatGpsGalileoQz1c:
      pData = "GPS+GALILEO+QZSS_L1CA";
      break;

    case eSatGpsGlonassQz1c:
    default:
      pData = "GPS+GLONASS+QZSS_L1CA";
      break;
  }
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%s\n", pComment, pParam, pData);
  ParamString += StringBuffer;

  /* Set NmeaOutUart. */
  pComment = "; Output NMEA message to UART(TRUE/FALSE)";
  pParam = "NmeaOutUart=";
  if (pConfigParam->NmeaOutUart == FALSE)
  {
    pData = "FALSE";
  }
  else
  {
    pData = "TRUE";
  }
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%s\n", pComment, pParam, pData);
  ParamString += StringBuffer;

  /* Set NmeaOutFile. */
  pComment = "; Output NMEA message to file(TRUE/FALSE)";
  pParam = "NmeaOutFile=";
  if (pConfigParam->NmeaOutFile == FALSE)
  {
    pData = "FALSE";
  }
  else
  {
    pData = "TRUE";
  }
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%s\n", pComment, pParam, pData);
  ParamString += StringBuffer;

  /* Set BinaryOut. */
  pComment = "; Output binary data to file(TRUE/FALSE)";
  pParam = "BinaryOut=";
  if (pConfigParam->BinaryOut == FALSE)
  {
    pData = "FALSE";
  }
  else
  {
    pData = "TRUE";
  }
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%s\n", pComment, pParam, pData);
  ParamString += StringBuffer;

  /* Set IntervalSec. */
  pComment = "; Positioning interval sec(1-300)";
  pParam = "IntervalSec=";
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%ld\n", pComment, pParam, pConfigParam->IntervalSec);
  ParamString += StringBuffer;

  /* Set ActiveSec. */
  pComment = "; Positioning active sec(60-300)";
  pParam = "ActiveSec=";
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%ld\n", pComment, pParam, pConfigParam->ActiveSec);
  ParamString += StringBuffer;

  /* Set SleepSec. */
  pComment = "; Positioning sleep sec(0-240)";
  pParam = "SleepSec=";
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%ld\n", pComment, pParam, pConfigParam->SleepSec);
  ParamString += StringBuffer;

  /* Set UartDebugMessage. */
  pComment = "; Uart debug message(NONE/ERROR/WARNING/INFO)";
  pParam = "UartDebugMessage=";
  switch (pConfigParam->UartDebugMessage)
  {
    case PrintError:
      pData = "ERROR";
      break;

    case PrintWarning:
      pData = "WARNING";
      break;

    case PrintInfo:
      pData = "INFO";
      break;

    case PrintNone:
    default:
      pData = "NONE";
      break;
  }
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%s\n%s%s\n", pComment, pParam, pData);
  ParamString += StringBuffer;

  /* End of file. */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "; EOF");
  ParamString += StringBuffer;

  return ParamString;
}

/**
 * @brief Read the ini file and set it as a parameter.
 * 
 * @details If there is no description, it will be the default value.
 * @param [out] pConfigParam Configuration parameters
 * @return 0 if success, -1 if failure
 */
static int ReadParameter(ConfigParam *pConfigParam)
{
  char *pReadBuff = NULL;
  pReadBuff = (char*)malloc(CONFIG_FILE_SIZE);
  if (pReadBuff == NULL)
  {
    APP_PRINT_E("alloc error:");
    APP_PRINT_E(CONFIG_FILE_NAME);
    APP_PRINT_E("\n");

    return -1;
  }

  /* Read file. */
  int ReadSize = 0;
  ReadSize = ReadChar(pReadBuff, CONFIG_FILE_SIZE, CONFIG_FILE_NAME, FILE_READ);
  if (ReadSize == 0)
  {
    APP_PRINT_E("read error:");
    APP_PRINT_E(CONFIG_FILE_NAME);
    APP_PRINT_E("\n");

    return -1;
  }

  /* Set NULL at EOF. */
  pReadBuff[ReadSize] = NULL;

  /* Record the start position for each line. */
  int CharCount;
  int LineCount = 0;
  boolean FindSeparator = true;
  char *LineList[128] = {0,};

  for (CharCount = 0; CharCount < ReadSize; CharCount++)
  {
    if (FindSeparator == true)
    {
      LineList[LineCount] = &pReadBuff[CharCount];
      FindSeparator = false;
      LineCount++;
      if (LineCount >= 128)
      {
        break;
      } else {
        /* nop */
      }
    }

    if (pReadBuff[CharCount] == SEPARATOR)
    {
      FindSeparator = true;
      pReadBuff[CharCount] = NULL;
    }
  }

  /* Parse each line. */
  int MaxLine = LineCount;
  char *pParamName;
  char *pParamData;
  int length;
  int tmp;
  for (LineCount = 0; LineCount < MaxLine; LineCount++)
  {
    pParamName = LineList[LineCount];

    pParamData = NULL;
    if (pParamName[0] != ';')
    {
      length = strlen(pParamName);

      for (CharCount = 0; CharCount < length; CharCount++)
      {
        if (pParamName[CharCount] == '=')
        {
          pParamData = &(pParamName[CharCount + 1]);
          break;
        }
      }
    }

    /* Parse start. */
    if (pParamData == NULL)
    {
      /* nop */
    }
    else if (!ParamCompare(pParamName, "SatelliteSystem="))
    {
      if (!ParamCompare(pParamData, "GPS+GLONASS+QZSS_L1CA"))
      {
        pConfigParam->SatelliteSystem = eSatGpsGlonassQz1c;
      }
      else if (!ParamCompare(pParamData, "GPS+BEIDOU+QZSS_L1CA"))
      {
        pConfigParam->SatelliteSystem = eSatGpsBeidouQz1c;
      }
      else if (!ParamCompare(pParamData, "GPS+GALILEO+QZSS_L1CA"))
      {
        pConfigParam->SatelliteSystem = eSatGpsGalileoQz1c;
      }
      else if (!ParamCompare(pParamData, "GPS+QZSS_L1CA+QZSS_L1S"))
      {
        pConfigParam->SatelliteSystem = eSatGpsQz1cQz1S;
      }
      else if (!ParamCompare(pParamData, "GPS+QZSS_L1CA"))
      {
        pConfigParam->SatelliteSystem = eSatGpsQz1c;
      }
      else if (!ParamCompare(pParamData, "GPS+GLONASS"))
      {
        pConfigParam->SatelliteSystem = eSatGpsGlonass;
      }
      else if (!ParamCompare(pParamData, "GPS+BEIDOU"))
      {
        pConfigParam->SatelliteSystem = eSatGpsBeidou;
      }
      else if (!ParamCompare(pParamData, "GPS+GALILEO"))
      {
        pConfigParam->SatelliteSystem = eSatGpsGalileo;
      }
      else if (!ParamCompare(pParamData, "GLONASS"))
      {
        pConfigParam->SatelliteSystem = eSatGlonass;
      }
      else if (!ParamCompare(pParamData, "GPS+SBAS"))
      {
        pConfigParam->SatelliteSystem = eSatGpsSbas;
      }
      else if (!ParamCompare(pParamData, "GPS"))
      {
        pConfigParam->SatelliteSystem = eSatGps;
      }
      else
      {
        pConfigParam->SatelliteSystem = eSatGpsGlonassQz1c;
      }
    }
    else if (!ParamCompare(pParamName, "NmeaOutUart="))
    {
      if (!ParamCompare(pParamData, "FALSE"))
      {
        pConfigParam->NmeaOutUart = false;
      }
      else
      {
        pConfigParam->NmeaOutUart = true;
      }
    }
    else if (!ParamCompare(pParamName, "NmeaOutFile="))
    {
      if (!ParamCompare(pParamData, "FALSE"))
      {
        pConfigParam->NmeaOutFile = false;
      }
      else
      {
        pConfigParam->NmeaOutFile = true;
      }
    }
    else if (!ParamCompare(pParamName, "BinaryOut="))
    {
      if (!ParamCompare(pParamData, "FALSE"))
      {
        pConfigParam->BinaryOut = false;
      }
      else
      {
        pConfigParam->BinaryOut = true;
      }
    }
    else if (!ParamCompare(pParamName, "IntervalSec="))
    {
      tmp = strtoul(pParamData, NULL, 10);
      pConfigParam->IntervalSec = max(1, min(tmp, 300));
    }
    else if (!ParamCompare(pParamName, "ActiveSec="))
    {
      tmp = strtoul(pParamData, NULL, 10);
      pConfigParam->ActiveSec = max(60, min(tmp, 300));
    }
    else if (!ParamCompare(pParamName, "SleepSec="))
    {
      tmp = strtoul(pParamData, NULL, 10);
      pConfigParam->SleepSec = max(0, min(tmp, 240));
    }
    else if (!ParamCompare(pParamName, "UartDebugMessage="))
    {
      if (!ParamCompare(pParamData, "NONE"))
      {
        pConfigParam->UartDebugMessage = PrintNone;
      }
      else if (!ParamCompare(pParamData, "ERROR"))
      {
        pConfigParam->UartDebugMessage = PrintError;
      }
      else if (!ParamCompare(pParamData, "WARNING"))
      {
        pConfigParam->UartDebugMessage = PrintWarning;
      }
      else if (!ParamCompare(pParamData, "INFO"))
      {
        pConfigParam->UartDebugMessage = PrintInfo;
      }
    }
  }

  return OK;
}

/**
 * @brief Create an ini file based on the current parameters.
 * 
 * @param [in] pConfigParam Configuration parameters
 * @return 0 if success, -1 if failure
 */
static int WriteParameter(ConfigParam *pConfigParam)
{
  String ParamString;
  int ret = -1;
  unsigned long write_size;

  /* Make parameter data. */

  ParamString = MakeParameterString(pConfigParam);

  /* Write parameter data. */

  if (strlen(ParamString.c_str()) != 0)
  {
    Led_isSdAccess(true);
    write_size = WriteChar(ParamString.c_str(), CONFIG_FILE_NAME, FILE_WRITE);
    Led_isSdAccess(false);

    if (write_size == strlen(ParamString.c_str()))
    {
      ret = 0;
    }
    else
    {
      Led_isError(true);
    }
  }

  return ret;
}

/**
 * @brief Setup configuration parameters.
 * 
 * @return 0 if success, -1 if failure
 */
static int SetupParameter(void)
{
  int ret;
  String ParamString;

  /* Read parameter file. */
  ret = ReadParameter(&Parameter);
  if (ret != OK)
  {
    /* If there is no parameter file, create a new one. */
    ret = WriteParameter(&Parameter);
  }
  
  /* Print parameter. */
  ParamString = MakeParameterString(&Parameter);/*////                                            ////*/
  APP_PRINT(ParamString.c_str());
  APP_PRINT("\n\n");


  return ret;
}

/**
 * @brief Go to Sleep mode
 */
static void SleepIn(void)
{
  /* Turn off the LED. */
  APP_PRINT("Sleep ");
  ledOff(PIN_LED0);
  Gnss.stop();
  Gnss.end();

  // TBD:Further sleep processing will be implemented.
  APP_PRINT("in.\n");
}

/**
 * @brief Go to Active mode.
 */
static void SleepOut(void)
{
  APP_PRINT("Sleep ");
  // TBD:Further wakeup processing will be implemented.

  Gnss.begin();
  Gnss.start(HOT_START);
  APP_PRINT("out.\n");
}

/**
 * @brief Get file number.
 * 
 * @return File count
 */
unsigned long GetFileNumber(void)
{
  int FileCount;
  char IndexData[INDEX_FILE_SIZE];
  int ReadSize = 0;

  /* Open index file. */
  ReadSize = ReadChar(IndexData, INDEX_FILE_SIZE, INDEX_FILE_NAME, FILE_READ);
  if (ReadSize != 0)
  {
    /* Use index data. */
    FileCount = strtoul(IndexData, NULL, 10);
    FileCount++;

    Remove(INDEX_FILE_NAME);
  }
  else
  {
    /* Init file count. */
    FileCount = 1;
  }

  /* Update index.txt */
  snprintf(IndexData, sizeof(IndexData), "%08d", FileCount);
  WriteChar(IndexData, INDEX_FILE_NAME, FILE_WRITE);

  return FileCount;
}

/**
 * @brief Setup positioning.
 * 
 * @return 0 if success, 1 if failure
 */
static int SetupPositioning(void)
{
  int error_flag = 0;

  /* Set default Parameter. */
  Parameter.SatelliteSystem  = eSatGpsGlonassQz1c;
  Parameter.NmeaOutUart      = true;
  Parameter.NmeaOutFile      = true;
  Parameter.BinaryOut        = false;
  Parameter.IntervalSec      = DEFAULT_INTERVAL_SEC;
  Parameter.ActiveSec        = DEFAULT_ACTIVE_SEC;
  Parameter.SleepSec         = DEFAULT_SEEP_SEC;
  Parameter.UartDebugMessage = PrintNone;

  /* Mount SD card. */
  if (BeginSDCard() != true)
  {
    /* Error case.*/
    APP_PRINT_E("SD begin error!!\n");

    error_flag = 1;
  }
  else
  {
    /* Setup Parameter. */
    SetupParameter();
  }

  /* Set Gnss debug mode. */
  Gnss.setDebugMode(Parameter.UartDebugMessage);
  AppDebugPrintLevel = (AppPrintLevel)Parameter.UartDebugMessage;

  if (Gnss.begin(Serial) != 0)
  {
    /* Error case. */
    APP_PRINT_E("Gnss begin error!!\n");
    error_flag = 1;
  }
  else
  {
    APP_PRINT_I("Gnss begin OK.\n");

    switch (Parameter.SatelliteSystem)
    {
    case eSatGps:
      Gnss.select(GPS);
      break;

    case eSatGpsSbas:
      Gnss.select(GPS);
      Gnss.select(SBAS);
      break;

    case eSatGlonass:
      Gnss.select(GLONASS);
      break;

    case eSatGpsGlonass:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      break;

    case eSatGpsBeidou:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      break;

    case eSatGpsGalileo:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      break;

    case eSatGpsQz1c:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsQz1cQz1S:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      Gnss.select(QZ_L1S);
      break;

    case eSatGpsBeidouQz1c:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGalileoQz1c:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGlonassQz1c:
    default:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      Gnss.select(QZ_L1CA);
      break;
    }

    Gnss.setInterval(Parameter.IntervalSec);

    if (Gnss.start(HOT_START) != OK)
    {
      /* Error case. */
      APP_PRINT_E("Gnss start error!!\n");
      error_flag = 1;
    }
  }

  /* Create output file name. */
  FilenameTxt[0] = 0;
  FilenameBin[0] = 0;
  if ( (Parameter.NmeaOutFile == true) || (Parameter.BinaryOut == true) )
  {
    int FileCount = GetFileNumber();

    if (Parameter.NmeaOutFile == true)
    {
      /* Create a file name to store NMEA data. */
      snprintf(FilenameTxt, sizeof(FilenameTxt), "%08d.txt", FileCount);
    }
    if (Parameter.BinaryOut == true)
    {
      /* Create a file name to store binary data. */
      snprintf(FilenameBin, sizeof(FilenameBin), "%08d.bin", FileCount);
    }
  }

  return error_flag;
}
//////////////////////////////////////////////////


#include <chrono>

chrono::system_clock::time_point startTime,endTime;
//////gps setting end

#define BAUDRATE  9600
#define BAUDRATE2 38400

bool isOpened;
bool isClosed;
bool isSafe;
int safeCount = 0;
int gpsCount = 0;

///define pin
//led1
#define blue1 5
#define green1 6
#define red1 7
//led2
#define blue2 8
#define green2 10
#define red2 9
//force sensing
#define forcePin A0

//define about send
const String gpsSend = "";
const String picSend = "";
//const String 
String strIdo="34.8327034",strKeido="138.5168499",strActivity ="";

//define about receive
const char weatherRecv = 'w';

const char sunnyDay = '1';
const char cloudyDay = '2';
const char rainyDay = '3';

const char voiceOrder = 'd';
const String correctPass = "open";
const char reconnectUnity = 'c';

byte sendByte[1];

int weatherCount = 0;

const char gpsOrder = 'g';

char orderRecv;
String voiceRecv="";
char weatherDay = "";

///threashold
int openThresh = 300;
int closeThresh = 1000;
int openCnt = 10000000;


void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial){;}
  Serial2.begin(BAUDRATE2);
  while (!Serial2){;}

  while(Serial.available()){Serial.read();}
  while(Serial2.available()){Serial2.read();}

  //camera setting
  theCamera.begin();
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  theCamera.setStillPictureImageFormat(320,240,CAM_IMAGE_PIX_FMT_RGB565);
  
  //pin initialize  
  pinMode(blue1,OUTPUT);
  pinMode(green1,OUTPUT);
  pinMode(red1,OUTPUT);
  pinMode(blue2,OUTPUT);
  pinMode(green2,OUTPUT);
  pinMode(red2,OUTPUT);
  allOff();

  isOpened = false;
  isSafe = false;
  isClosed = false;

  
 //gpsの初期設定
 int error_flag = 0;
 char KeyRead[2] = {0, 0};

 /* Wait mode select. */
 sleep(3);

 /* Read key input. */
 KeyRead[0] = Serial.read();

 /* Convet to mode value. */
 Mode = strtoul(KeyRead, NULL, 10);

 switch (Mode) {
#if 0 // TBD:Not implemented
   case eModeShell:
     /* nop */
     break;
#endif

   case eModeNormal:
   default:
     error_flag = SetupPositioning();
     break;
 }

 SetActiveSec = INITIAL_ACTIVE_TIME;
 startTime = chrono::system_clock::now();
 endTime = chrono::system_clock::now();

}



void loop() {

  // 一定時間したらsafe状態が終わる
  if(safeCount < openCnt){
    safeCount += 1;
  }else{
    Serial.println("Safe mode is end");
    safeCount = 0;
    isSafe = false;
    ledOff(green1);
  }

  // Unityからのデータを受け取りそれに応じた処理を行う
  unityOrder();
  
  // 傘の開閉を識別
  recognizeOpen();

  // デバッグ用
  if(Serial.available() > 0){
    char readByte = Serial.read();
    Serial.println(readByte);
    if(readByte == 'p'){
      isOpened = true;
    }
  }

  gpsGetting();

  // 傘が開かれたらUnityにデータを送る
  if (isOpened){
    if (isSafe){
      sendActivity(); 
    }else if(!isSafe){
      Serial.println("Call takePicture()");
      CamImage img = theCamera.takePicture();
      sendActivity();
      delay(5000);
      sendPicture(img);
    }
    isOpened = false;
  }

}


// 関数群

// Unityからの指令に応じて処理を行う
void unityOrder(){
  if(Serial2.available()){
    orderRecv = 'x';
    orderRecv = Serial2.read();
    if(orderRecv == voiceOrder){
      Serial.println("disconnect from unity");
      recognizeVoice();
      
    }else if(orderRecv == gpsOrder){
      Serial.println("gps mode");
      sendGps();
    }else if(orderRecv == weatherRecv){
      Serial.println("weather mode");
      weatherInfo();
    }
  }
}

// 音声認識アプリからきたデータの処理
void recognizeVoice(){
  
  ledOn(blue1);
  delay(500);
  ledOff(blue1);

  while(Serial2.available() <= 0){;}

  voiceRecv = Serial2.readString();
    
  //succeed  /from arduino voice control
  if(voiceRecv == correctPass){
    Serial.println("canbrella is safe\nplease reconnect to unity");
    isSafe = true;
    ledOn(green1);
    delay(2000);
    ledOff(green1);
    safeCount = 0;    
    // break;
  //failed  /from arduino voice control
  }else{
    Serial.println("Incorrect password");
    ledOn(red1);
    delay(2000);
    ledOff(red1);
  }
}

// 傘が開かれたかどうか識別しisOpenedにbool値をいれる
void recognizeOpen(){
  int val = analogRead(forcePin);
  Serial.println(val);
  
  if(val > openThresh){
    ledOn(red1);
    ledOn(red2);
    Serial.println("Open umbrella");
    isOpened = true;
    // allOff();
  }else if(val < closeThresh){
    isClosed = true;
  }

}

// GPSデータだけをUnityに送る
void sendGps(){
  strActivity = "";
  strActivity += strIdo;
  strActivity += ",";
  strActivity += strKeido;
  Serial.println("Send Gps Data");
  for(int i = 0;i < strActivity.length();i++){
    sendByte[0] = strActivity.charAt(i);
    Serial2.write(sendByte,1);
  }
}

// GPSの値を取得する
void gpsGetting(){
  endTime = chrono::system_clock::now();
  if(1000.0 >(double)chrono::duration_cast<chrono::milliseconds>(endTime-startTime).count())return;
  startTime = endTime;
static int State = eStateActive;
  static int TimeOut = IDLE_ACTIVE_TIME;
  static bool PosFixflag = false;
  static char *pNmeaBuff     = NULL;
  static char *pBinaryBuffer = NULL;

  /* Check state. */
  if (State == eStateSleep)
  {
    /* Sleep. */
    TimeOut--;

    /* Counter Check. */
    if (TimeOut <= 0)
    {

      /* Set active timeout. */
      TimeOut = IDLE_ACTIVE_TIME;
      SetActiveSec = Parameter.ActiveSec;

      /* Set new mode. */
      State = eStateActive;

      /* Go to Active mode. */
      SleepOut();
    }

  }
  else
  {
    /* Active. */
    unsigned long BuffSize;
    unsigned long WriteSize;
    bool LedSet;

    TimeOut -= Parameter.IntervalSec;

    SpNavData NavData;
    String NmeaString = "";

    /* Blink LED. */
    Led_isActive();

    int WriteRequest = false;

    /* Check update. */
    if (Gnss.waitUpdate(Parameter.IntervalSec * 1000))
    {
      /* Get NavData. */
      Gnss.getNavData(&NavData);

      LedSet = ((NavData.posDataExist) && (NavData.posFixMode != FixInvalid));
      if(PosFixflag != LedSet)
      {
        Led_isPosfix(LedSet);
        PosFixflag = LedSet;

        if(LedSet == true)
        {
          TimeOut = SetActiveSec;
          WriteRequest = true;
        }
      }

      /* Get Nmea Data. */
      NmeaString = getNmeaGga(&NavData);
      if (strlen(NmeaString.c_str()) == 0)
      {
        /* Error case. */
        //APP_PRINT_E("getNmea error");
        Led_isError(true);
      }
      else
      {
        /* Output Nmea Data. */
        if (Parameter.NmeaOutUart == true)
        {
          
          /* To Uart. */
          //APP_PRINT(NmeaString.c_str());//ここで値をシリアルモニタに送信している///////////////////////////////////////////////////
          String num1[6];
          String gps,tmp;
          int k=0;

          //gps = "$GPGGA,075004.00,3606.6437,N,14005.9706,E,1,07,1.3,38.4,M,,M,,*43";
          //APP_PRINT(NmeaString.c_str());
          for(int i = 0;i < 6;i++)num1[i] = "";
          for(int i = 0;i < 6 ;i++){
            for(int j = 0;j < 20;j++){
              if(NmeaString[k] == ','){
                k++;
                break;
              }
              //num1[i] += gps[k];
              //APP_PRINT("");
              num1[i] += NmeaString[k];
              k++;
            }
            if(num1[2] == 0){
              //APP_PRINT("");
            }
          }
          k = 0;
          //APP_PRINT("");
          //APP_PRINT("\n");
          if(num1[2] == "")num1[2] = "3600.00";
          if(num1[4] == "")num1[4] = "14000.00";
          strIdo = num1[2];
          strKeido = num1[4];

          gpsCount += 1;
          if(gpsCount == 10){
            gpsCount = 0;
            tmp = num1[2] + ',' + num1[4];
            //Serial2.print(tmp.c_str());
            Serial.println("tmp");
            Serial.println(tmp.c_str());
            Serial.println("Nmea");
            Serial.println(NmeaString);
          }
          
        }

        if (Parameter.NmeaOutFile == true)
        {
          /* To SDCard. */
          BuffSize = NMEA_BUFFER_SIZE * (IDLE_ACTIVE_TIME / Parameter.IntervalSec);

          if (pNmeaBuff == NULL)
          {
            /* Alloc buffer. */
            pNmeaBuff = (char*)malloc(BuffSize);
            if (pNmeaBuff != NULL)
            {
              /* Clear Buffer */
              pNmeaBuff[0] = 0x00;
            }
          }

          if (pNmeaBuff != NULL)
          {
            /* Store Nmea Data to buffer. */
            strncat(pNmeaBuff, NmeaString.c_str(), BuffSize);
          }
        }

        /* Output Binary Data. */
        if (Parameter.BinaryOut == true)
        {
          BuffSize = Gnss.getPositionDataSize();
          if (pBinaryBuffer == NULL)
          {
            /* Alloc buffer. */
            pBinaryBuffer = (char*)malloc(Gnss.getPositionDataSize());
            if (pBinaryBuffer != NULL)
            {
              /* Clear Buffer. */
              pBinaryBuffer[0] = 0x00;
            }
          }
          if (pBinaryBuffer != NULL)
          {

            if (Gnss.getPositionData(pBinaryBuffer) == BuffSize)
            {
              /* Write Binary Data. */
              GnssPositionData *pAdr = (GnssPositionData*)pBinaryBuffer;
              Led_isSdAccess(true);
              WriteSize  = WriteBinary((char*)&pAdr->MagicNumber, FilenameBin, sizeof(pAdr->MagicNumber), (FILE_WRITE | O_APPEND));
              WriteSize += WriteBinary((char*)&pAdr->Data,        FilenameBin, sizeof(pAdr->Data),        (FILE_WRITE | O_APPEND));
              WriteSize += WriteBinary((char*)&pAdr->CRC,         FilenameBin, sizeof(pAdr->CRC),         (FILE_WRITE | O_APPEND));
              Led_isSdAccess(false);

              /* Check result. */
              if (WriteSize != BuffSize)
              {
                Led_isError(true);
              }
            }
          }
        }
      }
    }

    /* Counter Check. */
    if (TimeOut <= 0)
    {
      if (Parameter.SleepSec > 0)
      {
        /* Set new mode. */
        State = eStateSleep;

        /* Go to Sleep mode. */
        SleepIn();

        /* Set sleep timeout. */
        TimeOut = Parameter.SleepSec;
      }
      else
      {
        /* Set sleep timeout. */
        TimeOut = Parameter.SleepSec;
      }

      WriteRequest = true;
    }

    /* Check NMEA buffer. */
    if(strlen(pNmeaBuff) > (BuffSize - NMEA_BUFFER_SIZE))
    {
      /* There is no capacity for writing in the next NMEA. */
    }

    /* Write NMEA data. */
    if(WriteRequest == true)
    {
      if (pNmeaBuff != NULL)
      {
        /* Write Nmea Data. */
        Led_isSdAccess(true);
        WriteSize = WriteChar(pNmeaBuff, FilenameTxt, (FILE_WRITE | O_APPEND));
        Led_isSdAccess(false);

        /* Check result. */
        if (WriteSize != strlen(pNmeaBuff))
        {
          Led_isError(true);
        }

        /* Clear Buffer */
        pNmeaBuff[0] = 0x00;
      }
    }
  }
}

// アクティビティデータ送信
void sendActivity(){
  // アクティビティデータ送信の合図
  Serial.println("Send Activity Sign");
  sendByte[0] = 'a';
  for(int i=1; i<=20; ++i){
    Serial2.write(sendByte, 1);
  }

  delay(5000);

  // アクティビティデータ本体の送信
  if(isSafe)strActivity = "safe,";
  else strActivity = "danger,";
  strActivity += strIdo;
  strActivity += ",";
  strActivity += strKeido;
  strActivity += ",";
  Serial.println("Send Activity Data");
  for(int i = 0;i < strActivity.length();i++){
    sendByte[0] = strActivity.charAt(i);
    Serial2.write(sendByte,1);
  }
}

// 写真送信
void sendPicture(CamImage img){
  if (img.isAvailable()){
    // 写真送信の合図
    Serial.println("Start sending Image");
    sendByte[0] = 'p';
    for(int i=1; i<=20; ++i){
      Serial2.write(sendByte, 1);
    }

    delay(5000);

    img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    char *buf = img.getImgBuff();
    for (int i = 0; i < img.getImgSize(); ++i, ++buf) {
      while(Serial2.availableForWrite()<=10){;}
      sendByte[0] = *buf;
      Serial2.write(sendByte, 1);
      delayMicroseconds(100); //早くデータを送り過ぎると接続切れるので注意
      if(i%640 == 0){
        Serial.print("Send byte : ");
        Serial.println(i);
        delay(5);
      }
      
    }

    Serial.println("End sending Image");
  }else{
    Serial.println("Image is not available");
  }
}

// 天気情報の処理
void weatherInfo(){
  while(Serial2.available()<=0){Serial.println("waiting weather");}
  if(Serial2.available()){
    weatherDay = Serial2.read();
  }

  weatherCount += 1;
  if(weatherCount == 4){
    weatherCount = 0;
  }
  
  if(isOpened)return;

  allOff();
  if(weatherCount == 0){
    Serial.println("it is fine");
    ledOn(red1);
  }else if(weatherCount == 1){
    Serial.println("it is cloudy");
    ledOn(red1);
    ledOn(green1);
    ledOn(blue1);
  }else if(weatherCount == 2){
    Serial.println("it is rain");
    ledOn(blue1);
  }else{
    Serial.println("no weather data");
  }

}

// 全てのLEDをOFFにする
void allOff(){
  digitalWrite(blue1,LOW);
  digitalWrite(green1,LOW);
  digitalWrite(red1,LOW);
  digitalWrite(blue2,LOW);
  digitalWrite(green2,LOW);
  digitalWrite(red2,LOW);
}
