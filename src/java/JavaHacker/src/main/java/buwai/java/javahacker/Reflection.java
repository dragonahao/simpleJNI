package buwai.javahacker;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Reflection {

	public static Object invokeStaticMethod(String class_name, String method_name, Class<?>[] pareTyple, Object[] pareVaules) {
		try {
			Class<?> obj_class = Class.forName(class_name);
			Method method = obj_class.getDeclaredMethod(method_name, pareTyple);
			method.setAccessible(true);
			return method.invoke(null, pareVaules);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
		return null;

	}

	/**
	 * 反射调用方法。
	 * 
	 * @param class_name
	 *            类名。
	 * @param method_name
	 *            方法名。
	 * @param obj
	 *            类对象。
	 * @param pareTyple
	 *            方法参数类型。
	 * @param pareVaules
	 *            方法参数值。
	 * @return
	 */
	public static Object invokeMethod(String class_name, String method_name, Object obj, Class<?>[] pareTyple, Object[] pareVaules) {

		try {
			Class<?> obj_class = Class.forName(class_name);
			Method method = obj_class.getDeclaredMethod(method_name, pareTyple);
			method.setAccessible(true);
			return method.invoke(obj, pareVaules);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
		return null;

	}

	public static Object getFieldOjbect(String class_name, String filedName, Object obj) {
		try {
			Class<?> obj_class = Class.forName(class_name);
			Field field = obj_class.getDeclaredField(filedName);
			field.setAccessible(true);
			return field.get(obj);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
		return null;

	}

	public static Object getFieldOjbect(Class<?> obj_class, String filedName, Object obj) {
		try {
			Field field = obj_class.getDeclaredField(filedName);
			field.setAccessible(true);
			return field.get(obj);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}
		return null;

	}

	public static Object getStaticFieldOjbect(String class_name, String filedName) {

		try {
			Class<?> obj_class = Class.forName(class_name);
			Field field = obj_class.getDeclaredField(filedName);
			field.setAccessible(true);
			return field.get(null);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
		return null;

	}

	public static void setFieldOjbect(String classname, String filedName, Object obj, Object filedVaule) {
		try {
			Class<?> obj_class = Class.forName(classname);
			Field field = obj_class.getDeclaredField(filedName);
			field.setAccessible(true);
			field.set(obj, filedVaule);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

	public static void setStaticOjbect(String class_name, String filedName, Object filedVaule) {
		try {
			Class<?> obj_class = Class.forName(class_name);
			Field field = obj_class.getDeclaredField(filedName);
			field.setAccessible(true);
			field.set(null, filedVaule);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}
	
}
