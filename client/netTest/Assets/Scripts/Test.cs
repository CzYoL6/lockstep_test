using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Test : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Client.instance.ConnectToServer("172.25.181.85", 2333);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
