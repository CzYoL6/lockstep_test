using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    private int m_Id;
    public int Id { get { return m_Id; } }
    public int hor = 0, ver = 0;
    private Vector2Int m_Pos;
    public Vector2Int position { get { return m_Pos; } }
    private GameObject m_ViewObject;
    private PlayerView m_ViewController;
    public int speed;

    public void InitPlayer(int id, Vector2Int initialPos) {
        m_Id = id;
        m_Pos = initialPos;
        m_ViewController = GetComponent<PlayerView>();
        m_ViewController.Init(this);
    }

    // Start is called before the first frame update
    void Start()
    {
        //TODO
        
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }


    public void HandlePlayerCmd(int frameId, Chat.ControlInfo_C_TO_S msg) {
        hor = ver = 0;
        
        if (msg.W) ver += 1;
        if (msg.S) ver -= 1;
        if (msg.A) hor -= 1;
        if (msg.D) hor += 1;
        Debug.LogWarning("handling player cmd ... " + "hor:" + hor + ", ver:" + ver);
    }

    public void HandleMove() {
        //Debug.Log("handling player move");
        m_Pos.x += speed * 66 * hor;
        m_Pos.y += speed * 66 * ver;
        hor = ver = 0;
    }

    public void EnterFrame(int frameId) {
        HandleMove();
    }
}
