package com.desikatta;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import android.os.Bundle;

import android.os.StrictMode;

import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class MainActivity extends AppCompatActivity implements SensorEventListener {

    // Various Sensor Class objects and variables.
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private float[] mRotationMatrix = new float[9];


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // to make full screen and remove title
        requestWindowFeature( Window.FEATURE_NO_TITLE );
        getSupportActionBar().hide(); //Hides App name..
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN );

        setContentView(R.layout.activity_main);

        //To allow networking in main thread.
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);

        try {
            storage.clientSocket = new DatagramSocket();
            storage.sendingData = new byte[13];
            storage.sendingDataBuffer = ByteBuffer.allocate(13).order(ByteOrder.LITTLE_ENDIAN);

        } catch (IOException e) {
            e.printStackTrace();
        }

        storage.calibrated = false; // TO MAKE  SURE all these are reset at entry
        storage.sensorOnFlag = false;
        storage.paired = false;
        storage.counter = 0;
    }

    @Override
    protected void onResume(){
        super.onResume();

        //To make sure, sensor is only activated when setup and calibration is done
        if (storage.sensorOnFlag) {
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(this);
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        mSensorManager.unregisterListener(this);
        try {
            storage.clientSocket.close();
        }catch (NullPointerException e){}
    }



    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    public void onSensorChanged(SensorEvent event) {
        if(storage.calibrated) {
            gatherAndSendData(event);
        }
        else {
            if(storage.counter==1000) {
                storage.calibrated = true;
                mSensorManager.unregisterListener(this);
            }else {
                storage.counter++;
                gatherAndSendData(event);
            }
        }
        //Log.i("tag", String.valueOf(storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil | storage.Nil_Nil_W_A_S_D_R_DPI));
    }



    /* gathers data stored in sensor buffer and send it to Server*/
    public void gatherAndSendData(SensorEvent event){
        SensorManager.getRotationMatrixFromVector(mRotationMatrix, event.values);
        SensorManager.getOrientation(mRotationMatrix, storage.AzimuthPitchRoll);
        try {
            storage.sendingDataBuffer.putFloat(0, storage.AzimuthPitchRoll[0]).putFloat(4,storage.AzimuthPitchRoll[1])
                    .putFloat(8, storage.AzimuthPitchRoll[2])
                    .put(12,(byte)(storage.Nil_Nil_W_A_S_D_R_DPI | storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil));
            storage.sendingData = storage.sendingDataBuffer.array();
            // Creating a UDP packet
            storage.sendingPacket = new DatagramPacket(storage.sendingData, storage.sendingData.length, storage.IPAddress, storage.SERVICE_PORT);
            // sending UDP packet to the server
            storage.clientSocket.send(storage.sendingPacket);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /* This function, takes entered IP from text view and store it in IP.
        Sends pairingpacket, to confirm connection.
         It's again used to start calibration.*/
    public void updateIPsendPairingPacketCalibrate(View view){
        if(storage.paired) {
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
        }
        else{
            try {
                Button calibrate = findViewById(R.id.ipButton);
                calibrate.setText(getString(R.string.calibrateButton));
                EditText editText = findViewById(R.id.ip);
                storage.IP = editText.getText().toString();
                TextView Text = findViewById(R.id.info);
                Text.setText("Trying to connect to server.... Watch PC for confirmation \n If Connection fails, click RECREATE button");
                storage.IPAddress = InetAddress.getByName(storage.IP);
                storage.sendingPacket = new DatagramPacket(storage.pairingPacket, storage.pairingPacket.length, storage.IPAddress, storage.SERVICE_PORT);
                storage.clientSocket.send(storage.sendingPacket);
                storage.paired = true;
            } catch (Exception e) {
                storage.paired = false;
            }
        }
    }

    //To recreate the app
    public void recreate(View view){
        recreate();
    }
    //TO exit the APP
    public void finish(View view) {
        finish();
        android.os.Process.killProcess(android.os.Process.myPid());
    }


    /*To start FUN i.e. Calibration is done and game is on.
      Loads control layout */
    public void startFun(View view){
        setContentView(R.layout.fun);

        storage.scrollRight = findViewById(R.id.scrollRight);
        storage.mouseButton1 = findViewById(R.id.mouseButton1);
        storage.mouseButton2 = findViewById(R.id.mouseButton2);
        storage.reload = findViewById(R.id.reload);

        storage.mDetector = new GestureDetector(this, new MyGestureListener());
        // Add a touch listener to the view
        // The touch listener passes all its events on to the gesture detector

        storage.scrollRight.setOnTouchListener(touchListener);
        storage.mouseButton1.setOnTouchListener(spray);
        storage.mouseButton2.setOnTouchListener(scope);
        storage.reload.setOnTouchListener(reload);
        storage.sensorOnFlag = true;
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
    }

    View.OnTouchListener reload = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch(event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI | 0b00000010);
                    return true;
                case MotionEvent.ACTION_UP:
                    storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI & 0b11111101);
                    return true;
            }
            return false;
        }
    };

    //to detect scope
    View.OnTouchListener scope = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch(event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil = (byte) (storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil | 0b01000000);
                    return true;
                case MotionEvent.ACTION_UP:
                    storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil = (byte) (storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil & 0b10111111);
                    return true;
            }
            return false;
        }
    };

    //detect mouse 1 and long press mouse 1;
    View.OnTouchListener spray = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch(event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil = (byte) (storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil | 0b10000000);
                    return true;
                case MotionEvent.ACTION_UP:
                    storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil = (byte) (storage.M1_M2_Nil_Nil_Nil_Nil_Nil_Nil & 0b01111111);
                    return true;
            }
            return false;
        }
    };

    // This touch listener passes everything on to the gesture detector.
    // That saves us the trouble of interpreting the raw touch events
    // ourselves.
    View.OnTouchListener touchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            // pass the events to the gesture detector
            // a return value of true means the detector is handling it
            // a return value of false means the detector didn't
            // recognize the event
            if(event.getAction() == MotionEvent.ACTION_UP){ // to reset keyboard key events
                storage.Nil_Nil_W_A_S_D_R_DPI = (byte)0x00;
                return true;
            }
            return storage.mDetector.onTouchEvent(event);
        }
    };
    }

