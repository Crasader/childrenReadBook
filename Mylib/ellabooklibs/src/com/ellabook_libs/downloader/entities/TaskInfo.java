package com.ellabook_libs.downloader.entities;

import java.io.File;
import java.io.Serializable;

/**
 * 任务实体类
 * Task entity.
 */
public class TaskInfo extends DLInfo implements Serializable {
    public int progress, length ,isfinish;

    public TaskInfo(String bookId,File dlLocalFile, String baseUrl, String realUrl, int progress, int length ,int isfinish) {
        super(bookId,dlLocalFile, baseUrl, realUrl);
        this.progress = progress;
        this.length = length;
        this.isfinish = isfinish;
    }
}
