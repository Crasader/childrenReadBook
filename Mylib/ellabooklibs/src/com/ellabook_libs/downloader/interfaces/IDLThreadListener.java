package com.ellabook_libs.downloader.interfaces;

/**
 * 下载线程监听器
 * 该监听仅供下载线程使用
 * Listener for download thread.
 * This Listener only used by download thread.
 */
public interface IDLThreadListener {
    void onThreadProgress(int progress);
}
