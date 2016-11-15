package com.ellabook_libs.downloader.interfaces;

import java.io.File;

import com.ellabook_libs.downloader.entities.TaskInfo;

/**
 * 下载监听器
 * Download listener.
 */
public class DLTaskListener {
    /**
     * 下载开始时回调 暂未使用
     *
     * @param fileName 文件名 File name.
     * @param url      文件下载地址 File length in byte.
     */
    public void onStart(String fileName, String url) {}

    /**
     * 网络连接时回调
     *
     * @param type 具体的网络类型{@link com.ellabook_libs.downloader.cons.PublicCons.NetType} type of network
     * @param msg  附加的连接信息 extra message of connect.
     * @return true表示连接正常 否则反之 true if connect success, otherwise is return false.
     */
    public boolean onConnect(int type, String msg) {
        return true;
    }

    /**
     * 下载进行时回调
     *
     * @param progress 当前的下载进度以100为最大单位 note:the max progress is 100.
     */
    public void onProgress(int progress , String type) {

    }


    /**
     * 下载进行时回调
     *
     * @param type 1是在下载队列，0为等待队列
     */
    public void onProgressType(int type ,String msg) {

    }
    
    /**
     * 下载停止时回调 暂未使用
     */
//    public void onStop() {
//
//    }

    /**
     * 解压失败时回调
     *
     */
    public void onUnZipFail(String msg) {

    }
    /**
     * 下载完成时回调
     *
     */
    public void onFinish(TaskInfo info) {

    }
    
    /**
     * 下载出错时回调
     *
     * @param error 具体的错误信息 error message.
     */
    public void onError(String error) {
    }
}
