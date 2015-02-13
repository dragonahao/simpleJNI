package buwai.sample.simplejni.inject;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	private native boolean inject(int pid);

	private EditText mtxtPid;
	private Button mbtnInject;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mtxtPid = (EditText) findViewById(R.id.txtPid);
		mbtnInject = (Button) findViewById(R.id.btnInject);
		
		mbtnInject.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				String text = mtxtPid.getText().toString().trim();
				if ("".equals(text)) {
					Toast.makeText(MainActivity.this, "«Î ‰»ÎPID", Toast.LENGTH_SHORT).show();
				} else {
					int pid = Integer.parseInt(text);
					// ◊¢»Î°£
					inject(pid);
				}
			}
		});
	}
	
	static {
		System.loadLibrary("SimpleJNIInjectSmaple");
	}
}
