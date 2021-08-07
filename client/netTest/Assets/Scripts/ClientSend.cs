using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using System.Xml.Serialization;
using Google.Protobuf;

public class ClientSend : MonoBehaviour
{
    private static void SendTCPData(Packet _packet) {
        _packet.WriteLength();
        Client.instance.tcp.SendData(_packet);
    }

    #region Packets
    public static void SetNickname(string nickname) {
        using (Packet _packet = new Packet((int)Chat.TYPE.SetNickNameCToS)) {
            Chat.SetNickName_C_TO_S msg = new Chat.SetNickName_C_TO_S {
                Nickname = nickname
            };
            _packet.Write(msg.ToByteArray());

            SendTCPData(_packet);
        }
    }


    #endregion

}
