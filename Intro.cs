using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Video;

public class Intro: MonoBehaviour{

    public VideoPlayer videoPlayer;
    public GameObject cutscene;
    public GameObject system;

    public GameObject timer;

    public int skiptimer= 30;

    float inputAcceleration = 0;

    bool done=false;


     void Start(){
        //get input acceleration and set it to zero
        inputAcceleration = GameObject.Find("EscRoom-Player Variant").GetComponent<OVRPlayerController>().Acceleration;//change
        GameObject.Find("EscRoom-Player Variant").GetComponent<OVRPlayerController>().Acceleration=0;//change
        timer.SetActive(false);

    }

    void Update(){
        if(done){
            //revert to input Acceleration if cutscene is done
            GameObject.Find("EscRoom-Player Variant").GetComponent<OVRPlayerController>().Acceleration = inputAcceleration;//change
            system.SetActive(false);
            timer.SetActive(true);
            

        }

        if ((ulong)videoPlayer.frame == videoPlayer.frameCount-1 || !cutscene.activeSelf){//change 
            //Video Finished
            done=true;
        }

    }


}
