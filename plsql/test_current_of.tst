PL/SQL Developer Test script 3.0
37
-- Created on 2011-4-11 by AFAN ，记录数：影响/网关/表 = 11000/65w/513w
declare
  -- Local variables here
  cursor curOnlineRole(n_GW_ID number) is
    select r.role_id, r.login_id, r.rowid
      from UMS_ROLE r
     where r.gateway_id = n_GW_ID
       and r.login_id > 0
       and r.role_state = 1;

  cursor curOnlineRole2(n_GW_ID number) is
    select r.role_id, r.login_id
      from UMS_ROLE r
     where r.gateway_id = n_GW_ID
       and r.login_id > 0
       and r.role_state = 1
       for update;

begin
  -- Test statements here
  if (:proc = 1) then
    dbms_output.put_line('1');
    for rowRole in curOnlineRole(601012) loop
      update UMS_ROLE set login_id = 0 where rowid = rowRole.rowid;
      --update UMS_ROLE set login_id = 0 where role_id = rowRole.role_id; 这样写就变成了不可完成的任务
    end loop;
  else
    dbms_output.put_line('2');
    for rowRole in curOnlineRole2(601012) loop
      update UMS_ROLE set login_id = 0 where current of curOnlineRole2;
    end loop;
  end if;
  commit;
end;
-- 消耗时间(s)
-- 1.533, 1.406, 1.422, 1.422
-- 2.812, 2.859, 2.86,  2.828
1
proc
1
1
3
1
rowRole.role_id
