#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// 函数声明
int degreeToPulseTime(float V);
void serialEvent(int i,int value);
void captureCheck(int &value);

// 硬件配置
#define DHTPIN 4         
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t = 0, h = 0;                            // 温度、湿度

/*
  servo.attach(数字引脚,[1000],[2000]);         // 连舵机黄线的引脚,脉冲信号一般设为 500微秒 到 2500微秒
  servo.attached();                            // <bool>获取舵机对象是否已经连接引脚
  servo.detach();                              // 断开引脚控制,使对象不再控制任何舵机

  servo.write(新角度值);                        // 控制转动舵机,数值<200视为角度,否则为脉冲时间(微秒)
  servo.read();                                // <int>获取最后一次write()命令的角度值,0-180 之间
  servo.writeMicroseconds(新脉冲值);            // 控制转动舵机,发送信号脉冲宽度值 500-2500 微秒之间
  servo.readMicroseconds();                    // <int>获取最后一次writeMicroseconds()命令脉冲值

  库文件Servo.cpp里值<200才视为角度,对270度的舵机建议用writeMicroseconds().注意:360度舵机不能控制角度的
  
  9克舵机从一个角度转到另一个角度需要时间,约120毫秒60度.从0度转到180度需要360毫秒,1秒=1000毫秒=100000微秒
  调用write() 5微秒就能返回,同时write()多个舵机,就能看到多舵机同时转动的效果.
  当我们希望舵机是一个接一个轮流转动,或者转到目标角度后再其他操作,可在调用write()后可以再适当的延时 delay();
*/
/*
   控制舵机的高电平脉冲信号在周期20000微秒里占用500-2500微秒时间控制舵机转到指定角度..模拟舵机要持续发信号直到目标角度..
    500微秒----------0度；   1秒=1000毫秒，1毫秒=1000微秒
   1000微秒---------45度；
   1500微秒---------90度；
   2000微秒--------135度；  
   2500微秒--------180度；
   高电平脉冲每 11.11微秒≈1度≈(2500-500)/180,所以1微秒≈0.1度  不用太较真..

   SG90,TS90A 空载转速:0.12秒/60度(4.8V)=120毫秒60度=2毫秒1度
   SG90 工作电压4.8V~6V  待机5ma,空载60ma,工作电流>300ma.最大负载电流700ma.  堵转扭矩:1.2~1.4公斤/厘米(4.8V)
   TS90A工作电压4.8V~6V  待机5ma,空载60ma,工作电流>300ma.最大负载电流700ma.  堵转扭矩:1.6~1.8公斤/厘米(4.8V)

   舵机三根电线  黄线->接信号(D0~D8),红线->接正极(5V),褐线->接负极(G)

*/

//开发板的数字针脚用来接6个电机.依次从底座到夹子为 XYZAB
#define xPin 26
#define yPin 27
#define zPin 14
#define aPin 12
#define bPin 13

// bool         Step = true;                      //true=减速(1角度1角度的转),false=0=舵机原速转动
float       factor=11.11;                      //角度转换脉冲宽度使用

//500=0度，1500=居中90度，2500=180度
// !!!!! Warning 注意 夹子的角度是 100（闭合）~ 10（张开）千万不能超出这个范围

// 松开一定为10 注意 夹子捕捉  宽度 3.5cm ~ 80 ; 宽度 2.9cm ~  83 ; 宽度 1.9cm ~ 89 加的角度不能太大 只能个位数的加
Servo servo[5];                                //创建舵机对象
char XYZAB[5] = {'X','Y','Z','A','B'};         //定义6个电机从底座到夹子为 XYZAB.
int rawdms[5] = { 90, 90, 90, 180, 10};        //上电初始化舵机归为的角度.
int olddms[5] = { 90, 90, 90, 90, 90};         //每次转动舵机后保存脉宽值到此,作为下次转动的起点.
int newdms[5] = { 90, 90, 90, 90, 90};         //每次转动舵机后保存脉宽值到此,作为下次转动的起点.
int mindms[5] = {  0,  0,  0,  0,  0};         //定义5个电机在机械臂中可转动的最小信号脉冲微秒值
int maxdms[5] = {180,180,180,180,180};         //定义5个电机在机械臂中可转动的最大信号脉冲微秒值

int pin[5] = {xPin,yPin,zPin,aPin,bPin};


// 网络配置
const char* ssid = "test";
const char* password = "123456789";
const char* serverAddress = "192.168.147.27";
const int serverPort = 1123;


// 核心1任务：wifi & dht11
void core1Task(void *pvParameters) {

    // WiFi连接
    WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        Serial.printf("wifi disconnect\r\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
      }

    WiFiClient client;
    char buffer[15];  // 固定缓冲区防止溢出‌
    
    while(1) {

      while (WiFi.status() != WL_CONNECTED) {
        Serial.printf("wifi reconnect......\r\n");
        WiFi.begin(ssid, password);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
      }

      // 传感器数据读取（
      h = dht.readHumidity();
      t = dht.readTemperature();

      // TCP数据发送  连上TCP服务器才会发送数据
      if (client.connect(serverAddress, serverPort)) {
        if (isnan(h) || isnan(t)) {  // 有效性校验  
          snprintf(buffer, sizeof(buffer), "th:%.2f,%.2f", t, h);
          Serial.printf("send: %s\r\n", buffer);
          client.print(buffer);
        }
          client.stop();
      }
      vTaskDelay(pdMS_TO_TICKS(5000));  // 非阻塞延时
    }
}

// 核心0任务：舵机控制
void core0Task(void *pvParameters) {

  while(1) {
    vTaskDelay(pdMS_TO_TICKS(5000));  // 非阻塞延时
  }
}



// 注意 开启双核必须加非阻塞延时 否则看门狗错误
void setup() {

    Serial.begin(115200);

    dht.begin();
    
    // 任务分配（堆栈优化）

    xTaskCreatePinnedToCore(core1Task,"Core1",4096,  NULL,1,NULL,1);

    xTaskCreatePinnedToCore(core0Task,"Core0",4096,  NULL,2,NULL,0);

    //-------------设置舵机数字插口与脉冲宽度微秒时间,驱动舵机到初始角度---------------

    //一个一个初始化 不会同时初始化的

    for(int i = 0; i < 5; i++ ){

      servo[i].attach(pin[i],500,2500);               //绑定针脚,设置信号脉冲宽度范围

      //初始化舵机使用数组,把角度全部转变为脉冲宽度值,并保存到数组里
      rawdms[i]=degreeToPulseTime((float)rawdms[i]);
      olddms[i]=rawdms[i];                            //rawdms变量拷贝给olddms变量
      newdms[i]=rawdms[i];                            //rawdms变量拷贝给newdms变量
      mindms[i]=degreeToPulseTime((float)mindms[i]);
      maxdms[i]=degreeToPulseTime((float)maxdms[i]);

      servo[i].write(rawdms[i]);                      //写入新角度值,控制舵机转动

      delay(500);                                     //等待该舵机转到目标角度.
    }
}


void parseData(String data) {
    int commaIndex = data.indexOf(',');  // 定位分隔符‌
    if (commaIndex == -1) {  // 有效性校验
        Serial.println("[ERROR] Invalid format");
        return;
    }
    
    // 数值提取（支持负数）
    int val1 = data.substring(0, commaIndex).toInt();
    int val2 = data.substring(commaIndex+1).toInt();

    // 应用场景示例
    serialEvent(val1, val2);
}

void loop() {

  String inputBuffer = "";  // 全局接收缓冲区

    while (Serial.available() > 0) {
        char inChar = Serial.read();
        if (inChar == '\n') {  // 以换行符为结束标志
            parseData(inputBuffer);
            inputBuffer = "";
        } else {
            inputBuffer += inChar;
        }
    }
}


//---------------把 0-180 角度值转为对应高电平脉冲信号时间宽度 ---------------
int degreeToPulseTime(float V) {                        //返回  把角度值转换为脉冲宽度值
    if(V < 360.0){                                      //值少于360视为角度,否则视为脉宽
      V = V*factor;    
      if( (float)( V - (int)V ) >= 0.45) V = V + 1.0;
      return 500 + (int)V;                              //180度舵机取值0~180间并转为脉宽
    } else {
      return constrain((int)V,500,2500);              //取值500~2500间,9克舵机都是这个
    }
}

//串口测试

// 舵机3,4,5 均返过来
void serialEvent(int i,int value) {

  if(i <= 0 || i >=6){
    return;
  }else if(value <0 || value > 180){
    return;
  }else{
    if( i == 5 ){
      captureCheck(value);
    }
    servo[i-1].write(degreeToPulseTime((float)value));
    delay(500);
  }

}

// 夹子捕获的输入角度检查 避免舵机卡转
void captureCheck(int &value)
{
  if(value > 100 || value < 10){
    value = 10; // 直接张到最大 避免卡住
  }
}


//--------------函数 Servo180( )----调用Servo.h里的功能控制舵机转动-------------
// bool Servo180(int servoCnt,int Value) {   //脉宽高电平 500微秒 到 2500微秒 之间，对应舵机0°～180°可转角度

//   int count=0;                            //对应的角度值

//   //------------------------------------------------------------------------

//   if( servoCnt >= 0 && I < servo_cnt ){

//     if(newdms[I]!=olddms[I]){             //检查上次设置的该舵机新角度是否已执行
//       Servo180(-1,0);                     //若未执行,则立即执行
//     }

//     newdms[I]=constrain(todms(Value),mindms[I],maxdms[I]); //转为脉宽数值存入

//   }

//   //----------------------------------------------

//   unsigned long ms=millis();                            //记录板子启动到现在的时间
//   int J=0,maxms=0;
//   do{                                                   
//     J=0;
//     for(I=0;I<ss;I++){                                  //检查所有舵机,找出要转动的
//       if(newdms[I]!=olddms[I]){                         //新旧值不同,需要发信号转动
//          int range=abs(newdms[I]-olddms[I]);            //计算当前转到目标的脉宽差
//          maxms=max(maxms,range);                        //支持多舵机同时转动,记下最耗时间
//          if(Step && range>(int)factor){                 //需要减速即逐角度转动
//              if(newdms[I]>olddms[I]) {                  
//                  olddms[I]+=(int)factor;                //正转1度
//              } else {
//                  olddms[I]-=(int)factor;                //反转1度
//              }
//              S[I].write(olddms[I]);                     //发送信号
//              delay(1);
//          } else {
//              S[I].write(newdms[I]);                     //原速转动,直接发送目标脉宽信号
//              olddms[I]=newdms[I];                       //保存新的脉宽
//          }
//       } else J++;
//     }
//   } while(J!=ss);

//   //空载速度：0.12秒/60度(4.8V) = 120毫秒60度 = 2毫秒1度 = 2000微秒1度 = 2000微秒转11脉宽(转1脉宽信号需要消耗182微秒)
//   ms=maxms/(int)factor*5+ms;
//   if(ms>millis()){     //检查目前已消耗的时间与转到目标角度需要的时间
//     delay(constrain(ms-millis(),0,800));
//   }
//   //yield()=delay(0)延时可避免长时间循环触发看门狗重启开发板.Soft WDT reset
//   //循环超过500MS易触发看门狗重启开发板..重置看门狗计数 ESP.wdtFeed();禁止看门狗 ESP.wdtDisable() 超过5秒触发硬件看门狗
//   //Config_rawdms();   //启动这个函数,会实时保存转动过的舵机角度,断电后可以恢复各舵机到这个角度.
//   return maxms!=0;     //有发送过舵机信号返回 true
// }


//往前夹 和 复原
