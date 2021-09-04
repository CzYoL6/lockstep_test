using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour

{
    public static GameManager Instance { get; private set; }

    public bool started;
    public Dictionary<int, Chat.UpdateInfo_S_TO_C> infos;
    public List<Player> playerLists;
    public int currentFrameID, newFrameID;

    public GameObject playerPrefab;
    // Start is called before the first frame update
    private void Awake() {
        if (Instance == null) Instance = this;
        
    }
    void Start()
    {
        started = false;
        infos = new Dictionary<int, Chat.UpdateInfo_S_TO_C>();
        playerLists = new List<Player>();
    }

    // Update is called once per frame
    void Update()
    {
        //Debug.LogWarning("playerlist count...: " + playerLists.Count);
    }

    public void StartGame() {
        started = true;
    }

    public void AddPlayer(int id) {
        GameObject newPlayerObject = Instantiate(playerPrefab, new Vector2(0, 0), Quaternion.identity);
        newPlayerObject.GetComponent<Player>().InitPlayer(id, new Vector2Int(0,id * 5 - 5));
        playerLists.Add(newPlayerObject.GetComponent<Player>());
        
    }

    public Player GetPlayerById(int id) {
        
        foreach (var player in playerLists) {
            //Debug.LogWarning("playerid:" + player.Id);
            if (player.Id == id) {
                return player;
            }
        }

        return null;
    }


    private void HandleFSPMessage(int frameId, Chat.ControlInfo_C_TO_S msg) {

        Player player = GetPlayerById(msg.PlayerID);
        if (player == null) {
            Debug.LogError("cannot find player" + msg.PlayerID);
            return;
        }

        player.HandlePlayerCmd(frameId, msg);
    }

    private void FixedUpdate() {
        if(!started) return;
        bool W = false, A = false, S = false, D = false;
        if (Input.GetKey(KeyCode.W)) W = true;
        if (Input.GetKey(KeyCode.A)) A = true;
        if (Input.GetKey(KeyCode.S)) S = true;
        if (Input.GetKey(KeyCode.D)) D = true;
        if(W ||A||S||D)
            ClientSend.SendControlInfo(W, A, S, D);
    }

    public void EnterFrame() {
        while (currentFrameID < newFrameID) {
            ++currentFrameID;
            Debug.Log("current frame id:" + currentFrameID);
            Debug.Log("infos count:" + infos.Count);
            var frameData = infos[currentFrameID];
            int count = frameData.ControlInfos.Count;
            Debug.Log("current frame controlinfo count:" + count);
            for (int index = 0; index < count; ++index) {
                var message = frameData.ControlInfos[index];
                HandleFSPMessage(currentFrameID, message);
            }

            count = playerLists.Count;
            for (int i = 0; i < count; ++i) {
                var player = playerLists[i];
                player.EnterFrame(currentFrameID);
            }
        }
    }


}
