package com.ellabook.wxapi;


import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.json.JSONObject;

import com.ellabook.R;
import com.tencent.mm.sdk.constants.ConstantsAPI;
import com.tencent.mm.sdk.modelbase.BaseReq;
import com.tencent.mm.sdk.modelbase.BaseResp;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.IWXAPIEventHandler;
import com.tencent.mm.sdk.openapi.WXAPIFactory;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.text.style.BulletSpan;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

public class WXPayEntryActivity extends Activity implements IWXAPIEventHandler {

    private static final String TAG = "show";
    private boolean result = false;
    private IWXAPI api;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.e(TAG, "onCreate");
        setContentView(R.layout.pay_result);
        api = WXAPIFactory.createWXAPI(this, "wx612d474d79028d9b");
        api.handleIntent(getIntent(), this);
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
        api.handleIntent(intent, this);
    }

    @Override
    public void onReq(BaseReq req) {
    }

    @Override
    public void onResp(final BaseResp resp) {
        Log.e(TAG, "onPayFinish, errCode = " + resp.errCode);
        if (resp.getType() == ConstantsAPI.COMMAND_PAY_BY_WX) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("系统提示");
            builder.setOnKeyListener(keylistener).setCancelable(false);
            builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    switch (which) {
                        case AlertDialog.BUTTON_POSITIVE:// "确认"按钮退出程序
                            finish();
                            callback();
                            break;
                    }
                }
            });
            if (resp.errCode == 0) {
                builder.setMessage("支付成功");
                result = true;
            } else if (resp.errCode == -1) {
                builder.setMessage("支付异常,请先正常登陆微信");
            } else if (resp.errCode == -2) {
                builder.setMessage("取消支付");
            }
            builder.create().show();
        }
    }

    OnKeyListener keylistener = new DialogInterface.OnKeyListener() {
        public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
            if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
                finish();
                callback();
                return true;
            } else {
                return false;
            }
        }
    };

    private void callback() {
        String callstr;
        if (result)
            callstr = "3";
        else
            callstr = "4";
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("call", callstr);
            AppActivity.CrossPlatformCallback(jsonObject.toString());
        } catch (Exception e) {
            Log.e("201705071506", e.toString());
        }
    }
}