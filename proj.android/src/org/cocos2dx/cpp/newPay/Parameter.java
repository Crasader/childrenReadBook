package org.cocos2dx.cpp.newPay;

/**
 * Created by java on 2017/5/6.
 */

class Parameter {
    private String memberId;
    private String cardType;
    private String payType;
    private String platformName;
    private String resource;
    private String version;

    public Parameter(String memberId, String cardType, String payType, String platformName, String resource, String version) {
        this.memberId = memberId;
        this.cardType = cardType;
        this.payType = payType;
        this.platformName = platformName;
        this.resource = resource;
        this.version = version;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public String getCardType() {
        return cardType;
    }

    public void setCardType(String cardType) {
        this.cardType = cardType;
    }

    public String getPayType() {
        return payType;
    }

    public void setPayType(String payType) {
        this.payType = payType;
    }

    public String getPlatformName() {
        return platformName;
    }

    public void setPlatformName(String platformName) {
        this.platformName = platformName;
    }

    public String getResource() {
        return resource;
    }

    public void setResource(String resource) {
        this.resource = resource;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }
}
