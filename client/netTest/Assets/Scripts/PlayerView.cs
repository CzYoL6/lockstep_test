using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerView : MonoBehaviour
{
    private Player m_Player;

    // Start is called before the first frame update
    void Start()
    {
    }

    public void Init(Player player)
    {
        m_Player = player;
    }

    // Update is called once per frame
    void Update()
    {
        if (!GameManager.Instance.started) return;
        Vector2Int pos = m_Player.position;

        float x = pos.x * 0.001f;
        float y = pos.y * 0.001f;
        //Debug.Log("x, y:" + x + " " + y);
        // this.transform.position = new Vector3(x, transform.position.y, y);
        //this.transform.position = Vector2.Lerp(this.transform.position, new Vector2(x,y), .2f);
        this.transform.position = Vector2.MoveTowards(this.transform.position, new Vector2(x, y), m_Player.speed * Time.deltaTime );
    }
}
