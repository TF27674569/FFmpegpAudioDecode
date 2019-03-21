package com.sample.audiod;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.sample.audiod.ffmpegp.AudioUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void start(View view) {
        File file = new File(Environment.getExternalStorageDirectory().getPath(),"audioinput.m4a");
        Log.e("TAG", file.getAbsolutePath() + "   exists = " + file.exists() );

        AudioUtils.decode(file.getAbsolutePath(),new File(Environment.getExternalStorageDirectory().getPath(),"zzzz.pcm").getAbsolutePath());
    }
}
