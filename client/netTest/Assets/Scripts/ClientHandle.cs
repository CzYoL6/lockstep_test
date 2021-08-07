using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ClientHandle : MonoBehaviour
{
    public static void Welcome(Packet _packet) {
        Debug.Log($"Receive welcome msg from server ");
        Chat.Welcome_S_TO_C msg = Chat.Welcome_S_TO_C.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));
        Debug.Log("playerid: "+msg.PlayerID);

        
        Client.instance.myId = msg.PlayerID;
        //send welcome received packet
        ClientSend.SetNickname("www");
    }

    public static void NicknameSet(Packet _packet) {
        Debug.Log($"Nickname has been successfully set.");
        //Chat.NickName_Been_Set_S_TO_C msg = Chat.NickName_Been_Set_S_TO_C.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));


        //Client.instance.myId = _myId;
        //send welcome received packet

        //ClientSend.WelcomeReceived();
    }
}
