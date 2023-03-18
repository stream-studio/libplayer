package studio.stream.player

import android.os.Bundle
import android.util.Log
import android.view.SurfaceHolder
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import studio.stream.libplayer.Player
import studio.stream.libplayer.PlayerSurfaceView
import studio.stream.player.R


class PlayerMainActivity : AppCompatActivity(), SurfaceHolder.Callback {

    var player: Player? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        try{
            Player.init(this);
       } catch (e : Exception) {
            Toast.makeText(this, e.message, Toast.LENGTH_LONG).show();
            finish();
        return;
       }
        player = Player()

        setContentView(R.layout.activity_studio_main)

        val sv = findViewById<PlayerSurfaceView>(R.id.surface_video) as PlayerSurfaceView
        val sh = sv.holder
        sh.addCallback(this)

    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        Log.d("player", "Surface created: " + holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        Log.d("player", "Surface changed: " + holder.surface)
        player?.start(holder.surface);

    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {

    }
}