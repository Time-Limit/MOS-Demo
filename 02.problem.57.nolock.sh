# 第一个参数是文件名
# 向文件末尾追加一百个数字

if [[ -z $1 ]]; then
  echo "intput filename please";
  exit 1;
fi

repeat=0
while True
do
  last=`tail -n 1 $1 2>&1`;
  if [[ -z `echo $last | grep "^[0-9][0-9]*$"` ]]; then
    last=0;
  fi
  if [ $repeat -ge 100 ]; then
    break;
  fi
  repeat=`expr ${repeat} + 1`;
  next=`expr ${last} + 1`;
  echo $next >> $1;
done
