using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Test : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Client.instance.ConnectToServer("192.168.178.246", 2333);
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        GameManager.Instance.EnterFrame();
    }
}
