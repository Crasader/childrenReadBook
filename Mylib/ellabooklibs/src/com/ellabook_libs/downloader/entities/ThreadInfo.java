package com.ellabook_libs.downloader.entities;

import java.io.File;
import java.io.Serializable;

/**
 * 线程实体类
 * Thread entity.
 */
public class ThreadInfo extends DLInfo implements Serializable {
    public String id;
    public int start, end;

    public ThreadInfo(String bookId,File dlLocalFile, String baseUrl, String realUrl, int start, int end, String id) {
        super(bookId,dlLocalFile, baseUrl, realUrl);
        this.start = start;
        this.end = end;
        this.id = id;
    }
}
