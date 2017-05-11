package org.cocos2dx.cpp.newPay;

/**
 * Created by java on 2017/5/6.
 */

public class publicParameter {
    private String method = "method";
    private String format = "format";
    private String app_key = "app_key";
    private String v = "v";
    private String sign = "sign";
    private String sign_method = "sign_method";
    private String timestamp = "timestamp";
    private String content="";

    public publicParameter(String method, String FORMAT, String APP_KEY, String VERSION, String SIGN, String SIGN_METHOD, String TIMESTAMP) {
        this.method = method;
        this.format = FORMAT;
        this.app_key = APP_KEY;
        this.v = VERSION;
        this.sign = SIGN;
        this.sign_method = SIGN_METHOD;
        this.timestamp = TIMESTAMP;
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public String getMethod() {
        return method;
    }

    public void setMethod(String method) {
        this.method = method;
    }

    public String getFormat() {
        return format;
    }

    public void setFormat(String format) {
        this.format = format;
    }

    public String getApp_key() {
        return app_key;
    }

    public void setApp_key(String app_key) {
        this.app_key = app_key;
    }

    public String getV() {
        return v;
    }

    public void setV(String v) {
        this.v = v;
    }

    public String getSign() {
        return sign;
    }

    public void setSign(String sign) {
        this.sign = sign;
    }

    public String getSign_method() {
        return sign_method;
    }

    public void setSign_method(String sign_method) {
        this.sign_method = sign_method;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}
