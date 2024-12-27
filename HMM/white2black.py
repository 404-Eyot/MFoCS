#-*- coding:utf-8 –*-
from urllib import parse
import re
from hmmlearn import hmm
import numpy as np
import matplotlib.pyplot as plt
 
import joblib
#处理参数值的最小长度
MIN_LEN=6
 
#其他字符
SEN=['<','>',',',':','\'','/',';','"','{','}','(',')']
 
def ischeck(str):
    if re.match(r'^(http)',str):
        return False
    for i, c in enumerate(str):
        # 排除中文干扰 只处理127以内的字符
        if ord(c) > 127 or ord(c) < 31:
            return False
        if c in SEN:
            return True
 
    return False
 
def etl(str):
    vers=[]
    for i, c in enumerate(str):
        c=c.lower()
        if   ord(c) >= ord('a') and  ord(c) <= ord('z'):
            vers.append([ord('A')])
        elif ord(c) >= ord('0') and  ord(c) <= ord('9'):
            vers.append([ord('N')])
        elif c in SEN:
            vers.append([ord('C')])
        else:
            vers.append([ord('T')])
    #print(vers)
    return np.array(vers)
 
def train(filename):
    X = [[0]]
    X_lens = [1]
 
    with open(filename, encoding='utf-8') as f:
        for line in f:
            line=line.strip('\n')
            line=parse.unquote(line)
            vers = etl(line)
            X=np.concatenate( [X,vers])
            X_lens.append(len(vers))
 
    remodel = hmm.GaussianHMM(n_components=3, covariance_type="full", n_iter=100)
    remodel.fit(X,X_lens)
    joblib.dump(remodel, "xss-train1.pkl")
 
    return remodel
 
def test_normal(filename):
    remodel = joblib.load("xss-train1.pkl")
    x = []
    y = []
    with open(filename, encoding='utf-8') as f:
        for line in f:
            line=line.strip('\n')
            line=parse.unquote(line)
            vers = etl(line)
            pro = remodel.score(vers)
            x.append(len(vers))
            y.append(pro)
 
    return x,y
 
def test(filename):
    remodel = joblib.load("xss-train1.pkl")
    x = []
    y = []
    with open(filename, encoding='utf-8') as f:
        for line in f:
            # 切割参数
            result = parse.urlparse(line)
            print('result:', result)
            # url解码
            query = parse.unquote(result.query)
            print('query:', query)
            params = parse.parse_qsl(query, True)
            for k, v in params:
                print('k:', k, 'v:', v, 'line:', line)
                if ischeck(v) and len(v) >=MIN_LEN :
                    vers = etl(v)
                    pro = remodel.score(vers)
                    x.append(len(vers))
                    y.append(pro)
 
    return x,y
 
if __name__ == '__main__':
    train('./good-xss-200000.txt')
    x1,y1=test('./good-xss-200000.txt')
    x2,y2=test('./xss-200000.txt') 
    print(len(y1), len(y2))
    fig,ax=plt.subplots()
    ax.set_xlabel('Line Length')
    ax.set_ylabel('HMM Score')
    ax.scatter(x2, y2, color='g', label="xss",marker='v')
    ax.scatter(x1, y1, color='r', label="good",marker='*')
    ax.legend(loc='best')
    plt.show()