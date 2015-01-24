package buwai.sample.simplejni.hook;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
	
	private native void hooks();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		hooks();
	}
	
	static {
		System.loadLibrary("SimpleJNIHookSmaple");
	}
}
