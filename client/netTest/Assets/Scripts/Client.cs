using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;

public class Client : MonoBehaviour
{
    public static Client instance;

    public static int dataBufferSize = 1024*20;
    public string ip;
    public int port;
    public int myId = 0;
    public string userName;
    public TCP tcp;

    private bool isConnected = false;

    private delegate void PacketHandler(Packet _packet);
    private static Dictionary<Chat.TYPE, PacketHandler> packetHandlers;

    public class TCP {

        public TcpClient socket;

        private NetworkStream stream;

        private Packet receivedData;

        private byte[] receiveBuffer;

        public void SendData(Packet _packet) {
            try {
               // _packet.InsertInt(instance.myId);
                if (socket != null) {
                    stream.BeginWrite(_packet.ToArray(), 0, _packet.Length(), null, null);
                }
            }
            catch (Exception _ex) {
                Disconnect();
                Debug.Log($"Error sending data via TCP: {_ex}");

            }
        }

        public void Connect(string _ip, int _port) {
            instance.ip = _ip;
            instance.port = _port;

            socket = new TcpClient {
                ReceiveBufferSize = dataBufferSize,
                SendBufferSize = dataBufferSize
            };

            

            receiveBuffer = new byte[dataBufferSize];

            socket.BeginConnect(instance.ip, instance.port, ConnectCallback, null);
        }

        private void ConnectCallback(IAsyncResult _result) {
            socket.EndConnect(_result);
            if (!socket.Connected) {
                return;
            }
            stream = socket.GetStream();

            receivedData = new Packet();

            stream.BeginRead(receiveBuffer, 0, dataBufferSize, ReceiveCallback, null);
        }

        private void ReceiveCallback(IAsyncResult _result) {
            Debug.Log("received some thing");
            ThreadManager.ExecuteOnMainThread(() => {
                try {
                    int _byteLength = stream.EndRead(_result);

                    Debug.Log("received length: " + _byteLength);

                    if (_byteLength <= 0) {
                        //disconnect,server is closed
                        Debug.Log("server closed.");
                        Disconnect();
                        return;
                    }

                    byte[] _data = new byte[_byteLength];
                    Array.Copy(receiveBuffer, _data, _byteLength);

                    receivedData.Reset(HandleData(_data));

                    stream.BeginRead(receiveBuffer, 0, dataBufferSize, ReceiveCallback, null);
                }
                catch (Exception _ex) {

                    Debug.Log($"Error receiving TCP data: {_ex}");
                    //disconnect, tcp error
                    Disconnect();
                }
            });
            
        }

        private bool HandleData(byte[] _data) {
            int _packetLength = 0;
            receivedData.SetBytes(_data);

            if (receivedData.UnreadLength() >= 4) {
                _packetLength = receivedData.ReadInt();
                if (_packetLength <= 0) {
                    return true;
                }
            }

            while (_packetLength > 0 && _packetLength <= receivedData.UnreadLength()) {
                byte[] _packetBytes = receivedData.ReadBytes(_packetLength);
                using (Packet _packet = new Packet(_packetBytes)) {

                    Chat.TYPE _packetId = (Chat.TYPE)_packet.ReadInt();
                    Debug.Log("type: " + _packetId);
                    packetHandlers[_packetId](_packet); 
                }

                _packetLength = 0;
                if(receivedData.UnreadLength() >= 4) {
                    _packetLength = receivedData.ReadInt();
                    if(_packetLength <= 0) {
                        return true;
                    }
                }

            }

            if(_packetLength <= 1) {
                return true;
            }

            return false;
        }

        private void Disconnect() {
            instance.Disconnect();

            stream = null;
            receiveBuffer = null;
            receivedData = null;
            socket = null;
        }



    }
    private void InitializeClientData() {

        packetHandlers = new Dictionary<Chat.TYPE, PacketHandler>() {
                {Chat.TYPE.WelcomeSToC, ClientHandle.Welcome },
            {Chat.TYPE.UpdateRoomInfoSToC, ClientHandle.UpdateRoomInfo },
            {Chat.TYPE.NotifyStartGameSToC, ClientHandle.NotifyStartGame },
            {Chat.TYPE.UpdateInfoSToC, ClientHandle.UpdateInfo }

            };
        Debug.Log("Initializing Data..");
    }


    private void Awake() {
        tcp = new TCP();
        DontDestroyOnLoad(gameObject);
        if (instance == null) {
            instance = this;
        }
        else if(instance != this) {
            Destroy(this);
        }
    }

    public void Disconnect() {
        if (isConnected) {
            isConnected = false;
            if(tcp.socket.Connected)
                tcp.socket.Close();
            Debug.Log("Disconnected from server.");
        }
        //GameManager.instance.Restart();
    }

    // Start is called before the first frame update
    void Start(){
        
    }

    public void ConnectToServer(string IP, int PORT) {
        
        InitializeClientData();

        
        tcp.Connect(IP,PORT);
        isConnected = true;
    }

    // Update is called once per frame
    void Update(){
        
    }

    private void OnApplicationQuit() {
        Disconnect();
    }
    private void OnApplicationPause(bool pause) {
#if UNITY_IPHONE || UNITY_ANDROID
        if (pause) Disconnect();
#endif
    }
}
