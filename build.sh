echo "syncing"
rsync -ac --out-format="   %n" --no-t --delete-after . root@192.168.7.2:/root/ttymidi/

case $1 in
    all)
        echo "make all"
        ssh root@192.168.7.2 "make -C /root/ttymidi/ all"
    ;;
    debug)
        echo "make debug"
        ssh root@192.168.7.2 "make -C /root/ttymidi/ debug"
    ;;
esac