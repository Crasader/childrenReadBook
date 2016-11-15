package com.ellabook_libs.downloader.entities;

import java.io.File;
import java.io.Serializable;

/**
 * 下载实体类
 * Download entity.
 */
public class DLInfo implements Serializable {
    public File dlLocalFile;
    public String baseUrl, realUrl;
    public String bookId;

    public DLInfo(String bookId,File dlLocalFile, String baseUrl, String realUrl) {
    	this.bookId = bookId;
        this.dlLocalFile = dlLocalFile;
        this.baseUrl = baseUrl;
        this.realUrl = realUrl;
    }
}
