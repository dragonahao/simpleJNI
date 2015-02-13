package buwai.sample.simplejni.inject;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends Activity {

	private final static String TAG = "debug";
	private final static String EXE_NAME = "myinject";
	private final static String SO_INJECTED = "libinjected.so";

	private EditText mtxtPid;
	private Button mbtnInject;
	private String mExePath;
	private String mSoPath;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// ��ҪAPP��rootȨ�ޣ�
		
		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		mtxtPid = (EditText) findViewById(R.id.txtPid);
		mbtnInject = (Button) findViewById(R.id.btnInject);
		
		mExePath = getFilesDir() + "/" + EXE_NAME;
		mSoPath = "/data/data/" + getPackageName() + "/lib/" + SO_INJECTED;
		if (false == new File(mSoPath).exists()) {
			Log.w(TAG, "�ļ������ڣ�" + mSoPath);
		}
		
		try {
			// ��ע������assetsĿ¼�п���������
			copyFormAssets(getApplicationContext(), EXE_NAME, mExePath);
			// ����ִ��Ȩ�ޡ�
			RootCommand(new String[] {"chmod", "777", mExePath});
		} catch (IOException e) {
			e.printStackTrace();
		}

		mbtnInject.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				String pid = mtxtPid.getText().toString().trim();
				if ("".equals(pid)) {
					Toast.makeText(MainActivity.this, "������PID", Toast.LENGTH_SHORT).show();
				} else {
					// ע�롣
					RootCommand(new String[] {mExePath, pid, mSoPath});
				}
			}
		});
	}

	/**
	 * ���������С�ͬ��log��
	 * 
	 * @param cmd
	 * @return
	 */
	public static int RootCommand(String[] args) {
		StringBuilder cmd = new StringBuilder();
		for (String arg : args) {
			cmd.append(arg);
			cmd.append(" ");
		}
		return RootCommand(cmd.toString());
	}

	/**
	 * ���������С�ͬ��log��
	 * 
	 * @param cmd
	 * @return
	 */
	public static int RootCommand(String cmd) {
		Process ps = null;
		int exitcode = -1;
		DataOutputStream os = null;
		BufferedReader br = null;
		try {
			ps = Runtime.getRuntime().exec("su");
			os = new DataOutputStream(ps.getOutputStream());
			os.writeBytes(cmd + "\n");
			os.writeBytes("exit\n");
			os.flush();
			ps.waitFor();
			exitcode = ps.waitFor();
			
			br = new BufferedReader(new InputStreamReader(ps.getInputStream()));
			StringBuffer sb = new StringBuffer();
			String line;
			while ((line = br.readLine()) != null) {
				sb.append(line).append("\n");
			}
			String result = sb.toString();
			Log.i(TAG, result);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		} finally {
			if (null != os) {
				try {
					os.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			if (null != br) {
				try {
					br.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		return exitcode;
	}
	
	/**
	 * ��assets�ļ��п����ļ���/data/data/[Ӧ�ð���]/filesĿ¼�¡�
	 * 
	 * @param context
	 *            ��ǰ�����ġ�
	 * @param fileName
	 *            ������ơ�
	 * @param outPath
	 *            ���·����
	 * @return ���ؿ�·����
	 * @throws IOException 
	 */
	public static void copyFormAssets(Context context, String fileName, String outPath) throws IOException {
		InputStream is = null;
		OutputStream os = null;
		AssetManager am = context.getAssets();
		try {
			is = am.open(fileName);
			os = new FileOutputStream(outPath);

			byte[] buffer = new byte[1024];
			int length;
			while ((length = is.read(buffer)) > 0) {
				os.write(buffer, 0, length);
			}
		} finally {
			try {
				if (is != null) {
					is.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			
			try {
				if (os != null) {
					os.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
