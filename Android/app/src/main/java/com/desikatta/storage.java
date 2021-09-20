package com.desikatta;

import android.view.GestureDetector;
import android.view.View;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;

/* Class to store system wide shared variables*/
class storage{
    // Sensor related variables
    static boolean sensorOnFlag = false;
    static boolean calibrated = false;
    static boolean paired = false;
    static int counter = 0;
    final static byte[] pairingPacket = {
            (byte)0xff, (byte)0x00, (byte)0x00, (byte)0xff,
            (byte)0xff, (byte)0x00, (byte)0x00, (byte)0xff,
            (byte)0xff, (byte)0x00, (byte)0x00, (byte)0xff,
            (byte)0x00
    };

    // Data variables, that are sent to server
    static byte Nil_Nil_W_A_S_D_R_DPI = (byte)0x00;
    static byte M1_M2_Nil_Nil_Nil_Nil_Nil_Nil = (byte)0x00;
    static float[] AzimuthPitchRoll = new float[3];

    // Socket related objects and variables.
    static  DatagramSocket clientSocket;
    final static int SERVICE_PORT = 50001;
    static String IP;
    static  InetAddress IPAddress;
    static ByteBuffer sendingDataBuffer;
    static byte[] sendingData;
    static DatagramPacket sendingPacket;

    static GestureDetector mDetector;
    static View scrollRight;
    static View mouseButton1;
    static View mouseButton2;
    static View reload;
}

