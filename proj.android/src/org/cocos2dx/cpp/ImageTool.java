package org.cocos2dx.cpp;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.provider.MediaStore.Images.ImageColumns;
import android.util.Log;
import android.view.Display;
import android.widget.ImageView;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 ** Created by yuexing on 2015/8/25.
 */
public class ImageTool {
	
	 //拍照
    public static void photograph(Activity context,String name)
    {
    	File storageDir = new File(getPicturePath(),name);
    	Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(storageDir));
        Log.i("拍照", storageDir.getPath());
        context.startActivityForResult(intent, 100);
    }
    
    //跳转相册选图片
    public static void getLocationPath(Activity context)
    {
    	 Intent intent = new Intent(Intent.ACTION_PICK);  
         intent.setType("image/*");//相片类型  
         context.startActivityForResult(intent, 10);  
         
//         if(requestCode == 10)
// 		{			
// 			if(data != null){
// 				Uri uri = data.getData();
// 				String pathName = ImageTool.getRealFilePath(m_activity, uri);
// 			}
// 		}
    }
    
    //获取拍照存放的相册路径
    public static String getPicturePath()
    {
    	String path = "";
    	try {
    		path = Environment.getExternalStoragePublicDirectory(
			        Environment.DIRECTORY_PICTURES
			    ).getCanonicalPath();
		} catch (IOException e) {
			e.printStackTrace();
		}    	
    	return path;
    }
    
  //获取下载目录
    public static String getDownloadPath()
    {
    	String path = "";
    	try {
    		path = Environment.getExternalStoragePublicDirectory(
			        Environment.DIRECTORY_DOWNLOADS
			    ).getCanonicalPath();
		} catch (IOException e) {
			e.printStackTrace();
		}    	
    	return path;
    }
	
    //根据URI压缩保存图片到相册
	public static void saveFileFromUri(String name, Uri uri,Context context, int reqWidth, int reqHeight,boolean IsMax)
	{
		Bitmap bp = readUri(uri, context, reqHeight, reqHeight, IsMax);
		Bitmap rp = toRoundBitmap(bp);
		saveFile(rp,name,AppActivity.GetMyAppDir()+File.separator+ "temp");
	}

    public static Bitmap decodeSampledBitmapFromResource(Resources res, int resId,
                                                         int reqWidth, int reqHeight) {

        // First decode with inJustDecodeBounds=true to check dimensions
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;// 获取资源图片
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeResource(res, resId, options);

        // Calculate inSampleSize
        options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight,false);

        // Decode bitmap with inSampleSize set
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeResource(res, resId, options);
    }
    
    public static Bitmap decodeSampledBitmapFromFile(String path, int reqWidth, int reqHeight) {

		final BitmapFactory.Options options = new BitmapFactory.Options();
		options.inPreferredConfig = Bitmap.Config.ARGB_8888;// 获取资源图片
		options.inJustDecodeBounds = true;
		BitmapFactory.decodeFile(path, options);
		options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight,false);
		options.inJustDecodeBounds = false;
		return BitmapFactory.decodeFile(path, options);
    }

    public static int calculateInSampleSize(
            BitmapFactory.Options options, int reqWidth, int reqHeight,boolean IsMax) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        Log.i("正常","前W"+width+"H"+height);
        int inSampleSize = 1;
        if (height > reqHeight || width > reqWidth) {
            if (IsMax)
            {
                int inSampleSize_h = (int) Math.ceil((float) height / (float) reqHeight);
                int inSampleSize_w = (int) Math.ceil((float)width / (float)reqWidth);
                inSampleSize = inSampleSize_h>inSampleSize_w?inSampleSize_h:inSampleSize_w;
                int zhishu = 0;
                while (inSampleSize!=(int) Math.pow(2,zhishu))
                {
                    if (inSampleSize>(int) Math.pow(2,zhishu))
                    {
                        ++zhishu;
                    }
                    else
                    {
                        ++inSampleSize;
                    }
                }
            }
            else
            {
                if (width > height)
                {
                    inSampleSize = (int) Math.ceil((float) height / (float) reqHeight);
                }
                else
                {
                    inSampleSize = (int) Math.ceil((float)width / (float)reqWidth);
                }
            }
        }
        Log.i("正常","后W"+reqWidth+"H"+reqHeight);
        return inSampleSize;
    }

    public static Bitmap readBitmap(Context context, int resId,int width,int height)
    {
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inPreferredConfig = Bitmap.Config.RGB_565;
        opt.outWidth=width;
        opt.outHeight=height;
        InputStream is = context.getResources().openRawResource(resId);
        return BitmapFactory.decodeStream(is, null, opt);
    }


    public static Bitmap readBitmap(Context context, int resId)
    {
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inPreferredConfig = Bitmap.Config.RGB_565;
        InputStream is = context.getResources().openRawResource(resId);
        return BitmapFactory.decodeStream(is, null, opt);
    }

    public static void Delete(Bitmap bmp)
    {
        if (!bmp.isRecycled())
        {
            bmp.recycle();
            System.gc();
        }
    }

    /**
     * 图片变为圆角
     * @param bitmap:传入的bitmap
     * @param pixels：圆角的度数，值越大，圆角越大
     * @return bitmap:加入圆角的bitmap
     */
    public static Bitmap toRoundCorner(Bitmap bitmap, int pixels)
    {
        if(bitmap == null)
            return null;
        Bitmap output = Bitmap.createBitmap(bitmap.getWidth(), bitmap.getHeight(), Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(output);
        final int color = 0xff424242;
        final Paint paint = new Paint();
        final Rect rect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());
        final RectF rectF = new RectF(rect);
        final float roundPx = pixels;
        paint.setAntiAlias(true);
        canvas.drawARGB(0, 0, 0, 0);
        paint.setColor(color);
        canvas.drawRoundRect(rectF, roundPx, roundPx, paint);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
        canvas.drawBitmap(bitmap, rect, rect, paint);
        return output;
    }

    public static int dip2px(Context context, float dipValue){
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int)(dipValue * scale + 0.5f);
    }

    public static int px2dip(Context context, float pxValue){
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int)(pxValue / scale + 0.5f);
    }

    //根据uri缩小图片转成bitmap输出
    public static Bitmap readUri(Uri uri,Context context, int reqWidth, int reqHeight,boolean IsMax) {
        try {
            String path = getRealFilePath(context,uri);
            Log.i("正常",uri.toString()+"路径="+path);
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inPreferredConfig = Bitmap.Config.ARGB_8888;// 获取资源图片
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(path, options);
            options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight,IsMax);
            options.inJustDecodeBounds = false;
            Log.i("正常", "比例" + options.inSampleSize);
            return BitmapFactory.decodeFile(path, options);
        }
        catch (Exception e)
        {
            Log.e("异常201511020932",e.toString());
            return null;
        }
    }
    
  //根据绝对地址读取图片缩小图片转成bitmap输出
    public static Bitmap getBitmapformPicturePathByCompaction(String path,Context context, int reqWidth, int reqHeight,boolean IsMax) {
        try {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inPreferredConfig = Bitmap.Config.ARGB_8888;// 获取资源图片
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(path, options);
            options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight,IsMax);
            options.inJustDecodeBounds = false;
            Log.i("正常", "比例" + options.inSampleSize);
            return BitmapFactory.decodeFile(path, options);
        }
        catch (Exception e)
        {
            Log.e("异常",e.toString());
            return null;
        }
    }
    
    /**
     * Try to return the absolute file path from the given Uri
     *
     * @param context
     * @param uri
     * @return the file path or null
     */
    public static String getRealFilePath( final Context context, final Uri uri ) {
        if ( null == uri ) return null;
        final String scheme = uri.getScheme();
        String data = null;
        if ( scheme == null )
            data = uri.getPath();
        else if ( ContentResolver.SCHEME_FILE.equals( scheme ) ) {
            data = uri.getPath();
        } else if ( ContentResolver.SCHEME_CONTENT.equals( scheme ) ) {
            Cursor cursor = context.getContentResolver().query( uri, new String[] { ImageColumns.DATA }, null, null, null );
            if ( null != cursor ) {
                if ( cursor.moveToFirst() ) {
                    int index = cursor.getColumnIndex( ImageColumns.DATA );
                    if ( index > -1 ) {
                        data = cursor.getString( index );
                    }
                }
                cursor.close();
            }
        }
        return data;
    }

    /**
     * 保存文件
     * @param bm
     * @param fileName
     * @throws IOException
     */
    public static void saveFile(Bitmap bm, String fileName,String path){
        File dirFile = new File(path);
        if(!dirFile.exists()){
            dirFile.mkdir();
        }
        File myCaptureFile = new File(path, fileName);
        BufferedOutputStream bos = null;
        try {
            bos = new BufferedOutputStream(new FileOutputStream(myCaptureFile));
            if(bm.compress(Bitmap.CompressFormat.PNG, 100, bos)) {
                bos.flush();
                bos.close();
            }
            else
            {
                throw new Exception("保存Bitmpap失败!");
            }
        } catch (Exception e) {
            Log.e("异常201511171002",e.toString());
        }
    }

    /**
     * 获取图片文件类型
     */
    public static String getImageType(String path)
    {
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inJustDecodeBounds = true; //确保图片不加载到内存
        BitmapFactory.decodeFile(path,opts);
        return opts.outMimeType;
    }

    //图片切成圆形
   	public static Bitmap toRoundBitmap(Bitmap bitmap) {
            int width = bitmap.getWidth();
            int height = bitmap.getHeight();
            float roundPx;
            float left,top,right,bottom,dst_left,dst_top,dst_right,dst_bottom;
            if (width <= height) {
                    roundPx = width / 2;
                    top = 0;
                    bottom = width;
                    left = 0;
                    right = width;
                    height = width;
                    dst_left = 0;
                    dst_top = 0;
                    dst_right = width;
                    dst_bottom = width;
            } else {
                    roundPx = height / 2;
                    float clip = (width - height) / 2;
                    left = clip;
                    right = width - clip;
                    top = 0;
                    bottom = height;
                    width = height;
                    dst_left = 0;
                    dst_top = 0;
                    dst_right = height;
                    dst_bottom = height;
            }
           
            Bitmap output = Bitmap.createBitmap(width,height, Config.ARGB_8888);
            Canvas canvas = new Canvas(output);
           
            final int color = 0xff424242;
            final Paint paint = new Paint();
            final Rect src = new Rect((int)left, (int)top, (int)right, (int)bottom);
            final Rect dst = new Rect((int)dst_left, (int)dst_top, (int)dst_right, (int)dst_bottom);
            final RectF rectF = new RectF(dst);

            paint.setAntiAlias(true);
           
            canvas.drawARGB(0, 0, 0, 0);
            paint.setColor(color);
            canvas.drawRoundRect(rectF, roundPx, roundPx, paint);

            paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));
            canvas.drawBitmap(bitmap, src, dst, paint);
            return output;
    }  	
	
}
