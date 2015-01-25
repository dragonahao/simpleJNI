package buwai.android.util;

import java.io.PrintWriter;
import java.lang.reflect.InvocationTargetException;

import android.app.Application;
import android.content.Intent;
import android.os.Debug;

/**
 * 这个类专门用于反射ActivityThread中的方法和字段。
 * 
 * @author buwai
 *
 */
public class ActivityThreadWrapper {

	private Object sCurrentActivityThread;

	public ActivityThreadWrapper() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		sCurrentActivityThread = Reflection.invokeStaticMethod("android.app.ActivityThread",
				"currentActivityThread", null, null);
	}

	/**
	 * 获得当前的ActivityThread对象。
	 * 
	 * @return 返回当前的ActivityThread对象。
	 */
	public Object currentActivityThread() {
		return sCurrentActivityThread;
	}

	/**
	 * 获得当前包名。
	 * 
	 * @return 返回当前包名。
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws SecurityException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 * @throws InvocationTargetException
	 */
	public String currentPackageName() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return (String) Reflection.invokeStaticMethod("android.app.ActivityThread",
				"currentPackageName", null, null);
	}

	/**
	 * 获得当前进程名。
	 * 
	 * @return 返回当前进程名。
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws SecurityException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 * @throws InvocationTargetException
	 */
	public String currentProcessName() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return (String) Reflection.invokeStaticMethod("android.app.ActivityThread",
				"currentProcessName", null, null);
	}

	/**
	 * 获得当前Application。
	 * 
	 * @return 返回当前Application。
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws SecurityException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 * @throws InvocationTargetException
	 */
	public Application currentApplication() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return (Application) Reflection.invokeStaticMethod("android.app.ActivityThread",
				"currentApplication", null, null);
	}

	/**
	 * 获得包管理器（IPackageManager类型）。
	 * 
	 * @return 返回包管理器。
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws SecurityException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 * @throws InvocationTargetException
	 */
	public Object getPackageManager() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return Reflection.invokeStaticMethod("android.app.ActivityThread", "getPackageManager",
				null, null);
	}

	public void dumpMemInfoTable(PrintWriter pw, Debug.MemoryInfo memInfo, boolean checkin,
			boolean dumpFullInfo, boolean dumpDalvik, int pid, String processName, long nativeMax,
			long nativeAllocated, long nativeFree, long dalvikMax, long dalvikAllocated,
			long dalvikFree) throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		Reflection.invokeStaticMethod("android.app.ActivityThread", "dumpMemInfoTable", null, null);
	}

	/**
	 * Return the Intent that's currently being handled by a BroadcastReceiver
	 * on this thread, or null if none.
	 * 
	 * @throws InvocationTargetException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 * @throws SecurityException
	 * @throws NoSuchMethodException
	 * @throws ClassNotFoundException
	 */
	public Intent getIntentBeingBroadcast() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return (Intent) Reflection.invokeStaticMethod("android.app.ActivityThread",
				"getIntentBeingBroadcast", null, null);
	}

	/**
	 * 
	 * @return （返回ActivityThread类型。）
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws SecurityException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 * @throws InvocationTargetException
	 */
	public Object systemMain() throws ClassNotFoundException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException,
			InvocationTargetException {
		return Reflection
				.invokeStaticMethod("android.app.ActivityThread", "systemMain", null, null);
	}

}
