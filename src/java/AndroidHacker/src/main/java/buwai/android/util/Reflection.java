package buwai.android.util;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * 反射调用专类。
 * 
 * @author buwai
 *
 */
public class Reflection {

	/**
	 * 调用静态方法。
	 * 
	 * @param className 类名。
	 * @param methodName 方法名。
	 * @param paramTypes 方法参数类型。
	 * @param paramValues 方法参数值。
	 * @return
	 * @throws ClassNotFoundException
	 * @throws SecurityException
	 * @throws NoSuchMethodException
	 * @throws InvocationTargetException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 */
	public static Object invokeStaticMethod(String className, String methodName,
			Class<?>[] paramTypes, Object[] paramValues) throws ClassNotFoundException,
			NoSuchMethodException, SecurityException, IllegalAccessException,
			IllegalArgumentException, InvocationTargetException {
		Class<?> objClass = Class.forName(className);
		Method method = objClass.getDeclaredMethod(methodName, paramTypes);
		method.setAccessible(true);
		return method.invoke(null, paramValues);
	}

	/**
	 * 反射调用方法。
	 * 
	 * @param className 类名。
	 * @param methodName 方法名。
	 * @param obj 类对象。
	 * @param paramTypes 方法参数类型。
	 * @param paramValues 方法参数值。
	 * @return
	 * @throws ClassNotFoundException
	 * @throws SecurityException
	 * @throws NoSuchMethodException
	 * @throws InvocationTargetException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 */
	public static Object invokeMethod(String className, String methodName, Object obj,
			Class<?>[] paramTypes, Object[] paramValues) throws ClassNotFoundException,
			NoSuchMethodException, SecurityException, IllegalAccessException,
			IllegalArgumentException, InvocationTargetException {
		Class<?> objClass = Class.forName(className);
		Method method = objClass.getDeclaredMethod(methodName, paramTypes);
		method.setAccessible(true);
		return method.invoke(obj, paramValues);
	}

	/**
	 * 获得字段值。
	 * 
	 * @param className 类名。
	 * @param filedName 字段名。
	 * @param obj 类对象。
	 * @return 返回字段的值。
	 * @throws ClassNotFoundException
	 * @throws NoSuchFieldException
	 * @throws SecurityException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 */
	public static Object getFieldOjbect(String className, String filedName, Object obj)
			throws ClassNotFoundException, NoSuchFieldException, SecurityException,
			IllegalArgumentException, IllegalAccessException {
		Class<?> objClass = Class.forName(className);
		Field field = objClass.getDeclaredField(filedName);
		field.setAccessible(true);
		return field.get(obj);
	}

	/**
	 * 获得字段值。
	 * 
	 * @param objClass 对象类型。
	 * @param filedName 字段名。
	 * @param obj 类对象。
	 * @return 返回字段的值。
	 * @throws NoSuchFieldException
	 * @throws SecurityException
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 */
	public static Object getFieldOjbect(Class<?> objClass, String filedName, Object obj)
			throws NoSuchFieldException, SecurityException, IllegalArgumentException,
			IllegalAccessException {
		Field field = objClass.getDeclaredField(filedName);
		field.setAccessible(true);
		return field.get(obj);
	}

	public static Object getStaticFieldOjbect(String className, String filedName)
			throws ClassNotFoundException, NoSuchFieldException, SecurityException,
			IllegalArgumentException, IllegalAccessException {
		Class<?> objClass = Class.forName(className);
		Field field = objClass.getDeclaredField(filedName);
		field.setAccessible(true);
		return field.get(null);
	}

	public static void setFieldOjbect(String classname, String filedName, Object obj,
			Object filedVaule) throws ClassNotFoundException, NoSuchFieldException,
			SecurityException, IllegalArgumentException, IllegalAccessException {
		Class<?> objClass = Class.forName(classname);
		Field field = objClass.getDeclaredField(filedName);
		field.setAccessible(true);
		field.set(obj, filedVaule);
	}

	public static void setStaticOjbect(String className, String filedName, Object filedVaule)
			throws ClassNotFoundException, NoSuchFieldException, SecurityException,
			IllegalArgumentException, IllegalAccessException {
		Class<?> objClass = Class.forName(className);
		Field field = objClass.getDeclaredField(filedName);
		field.setAccessible(true);
		field.set(null, filedVaule);
	}

}
