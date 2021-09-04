using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ClientHandle : MonoBehaviour
{
    public Player player;
    public static void Welcome(Packet _packet) {
        Debug.Log($"Receive welcome msg from server ");
        Debug.Log($"packet unread length: " + _packet.UnreadLength());
        Chat.Welcome_S_TO_C msg = Chat.Welcome_S_TO_C.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));
        Debug.Log("playerid: "+msg.PlayerID);
        Debug.Log("kcp conv: " + msg.KcpConv);
        Debug.Log("udp ip: " + msg.UdpAddress);
        Debug.Log("udp port: " + msg.UdpPort);

        Client.instance.myId = msg.PlayerID;
        //send welcome received packet
        ClientSend.SetNickname("www");
    }

    public static void UpdateRoomInfo(Packet _packet) {
        Debug.Log($"Receive roominfo update msg from server ");

        Debug.Log($"packet unread length: " + _packet.UnreadLength());
        Chat.UpdateRoomInfo_S_TO_C msg = Chat.UpdateRoomInfo_S_TO_C.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));
        Debug.Log("player count: " + msg.PlayerCount);

        UIManager.Instance.countText.text = "count: " + msg.PlayerCount.ToString();

        
    }

    public static void UpdateInfo(Packet _packet) {
        Debug.Log($"Receive Update info.");
        Chat.UpdateInfo_S_TO_C msg = Chat.UpdateInfo_S_TO_C.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));
        int frameID = msg.FrameID;
        Debug.Log("recv frameID: " + frameID);
        foreach(var info in msg.ControlInfos) {
            Debug.Log("id:" + info.PlayerID + ", wasd:" + info.W + info.A + info.S + info.D);
        }
        GameManager.Instance.infos.Add(frameID, msg);
        GameManager.Instance.newFrameID = frameID;

        //Client.instance.myId = _myId;
        //send welcome received packet

        //ClientSend.WelcomeReceived();
    }

    public static void NotifyStartGame(Packet _packet) {
        Debug.Log($"Receive notify start game.");
        Chat.NotifyStartGame msg = Chat.NotifyStartGame.Parser.ParseFrom(_packet.ReadBytes(_packet.UnreadLength()));
        var playerInfos = msg.PlayerInfos;
        Debug.Log("playerinfo player_count: " + playerInfos.Count);
        foreach(var playerInfo in playerInfos) {
            Debug.Log("player_name: " + playerInfo.Nickname + ", player_id: " + playerInfo.PlayerID );
            GameManager.Instance.AddPlayer(playerInfo.PlayerID);
        }
        //Debug.LogError("playerid:" + FindObjectOfType<Player>().Id);
        GameManager.Instance.started = true;
        //Client.instance.myId = _myId;
        //send welcome received packet

        //ClientSend.WelcomeReceived();
    }
}
