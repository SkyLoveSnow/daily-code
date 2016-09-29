#
# coding:utf-8
# python中实现ruby中的define_method
# in ruby
# RESOURCES = {
#     "vpn_service" => "/v2.0/vpn/vnpservice",
#     "vpn_ikepolicy" => "/v2.0/vpn/ikepolicies",
#     "vpn_ipsecpolicy" => "v2.0/vpn/ipsecpolices",
#     "ipsec_site_connection" => "/v2.0/vpn/ipsec-site-connection"
# }
# def create_resource(resource,url)
#     # do requrest 
# end

# RESOURCES.each_key do |resource|
#     define_method "create_#{resource}" do |url|
#         print "call method:#{__method__}:"
#         print "create #{resource}:POST #{url}\n"
#         create_resource(resource,url)
#     end
# end
# def test_ruby_ok 
#     RESOURCES.each_pair do |resource, url|
#         send("create_#{resource}",url)
#     end
# end
# test_ruby_ok

# 因为python中 function 对象是python中的“一等公民”，
# 所以无法拦截到，可以通过class的setattr为类动态添加方法
# 利用装饰器进行函数局部化，也可以用functools.partial局部化
# in python
RESOURCES = {
    "vpn_service":"/v2.0/vpn/vnpservice",
    "vpn_ikepolicy":"/v2.0/vpn/ikepolicies",
    "vpn_ipsecpolicy":"v2.0/vpn/ipsecpolices",
    "ipsec_site_connection":"/v2.0/vpn/ipsec-site-connection"
}
class Target(object):

    def create_resource(self,resource,url):
        pass

for resource in RESOURCES.iterkeys():
    def wrapper(resource):
        def _method_for_create(self,url=None):
            print "call method: create_{}".format(resource)
            print "create #{resource}:POST #{url}\n"
            self.create_resource(resource,url)
        return _method_for_create
    setattr(Target,"create_{}".format(resource),wrapper(resource))

def test_python_ok():
    target = Target()
    for resource,url in RESOURCES.iteritems():
        func_name = "create_{}".format(resource)
        if hasattr(target,func_name):
            func = getattr(target,func_name)
            func(url)

if __name__ == '__main__':
    test_python_ok()







