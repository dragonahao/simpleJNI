package buwai.sample.simplejni.hook;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import dalvik.system.DexClassLoader;

public class MainActivity extends Activity {

	private native void hooks();

	private native void call_fopen(String filePath);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		hooks();

		try {
			String filePath = copyToFilesFormAssets(getApplicationContext(), "HelloWorld.jar");
			File dexFile = new File(filePath);
			call_fopen(filePath);
			new DexClassLoader(filePath, dexFile.getParent(), null, getClassLoader());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * 从assets文件夹拷贝文件到/data/data/[应用包名]/files目录下。
	 * 
	 * @param context 当前上下文。
	 * @param fileName 库的名称。
	 * @return 返回库路径。
	 * @throws IOException
	 */
	public static String copyToFilesFormAssets(Context context, String fileName) throws IOException {
		String libDir = context.getFilesDir().getPath();
		String libPath = libDir + "/" + fileName;

		File dir = new File(libDir);
		if (false == dir.exists()) {
			dir.mkdir();
		}

		copyFormAssets(context, fileName, libPath);

		return libPath;
	}

	/**
	 * 从assets文件夹拷贝文件到/data/data/[应用包名]/files目录下。
	 * 
	 * @param context 当前上下文。
	 * @param fileName 库的名称。
	 * @param outPath 输出路径。
	 * @return 返回库路径。
	 * @throws IOException
	 */
	public static void copyFormAssets(Context context, String fileName, String outPath)
			throws IOException {
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

	static {
		System.loadLibrary("SimpleJNIHookSmaple");
	}
}
