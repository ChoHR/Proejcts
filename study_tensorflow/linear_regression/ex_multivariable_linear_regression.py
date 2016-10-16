import tensorflow as tf

x1_data = [1,0,3,0,5]
x2_data = [0,2,0,4,0]
y_data = [1,2,3,4,5]

w1 = tf.Variable(tf.random_uniform([1], -1.0, 1.0))
w2 = tf.Variable(tf.random_uniform([1], -1.0, 1.0))
b = tf.Variable(tf.random_uniform([1], -1.0, 1.0))

X1 = tf.placeholder(tf.float32)
X2 = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

hypothesis = w1 * X1 + w2 * X2 + b 
                
cost = tf.reduce_mean(tf.square(hypothesis - Y))

a = tf.Variable(0.1) # Learning rate, alpha
optimizer = tf.train.GradientDescentOptimizer(a)
train = optimizer.minimize(cost)

init = tf.initialize_all_variables()

sess = tf.Session()
sess.run(init)

for step in xrange(1001):
    sess.run(train, feed_dict={X1:x1_data, X2:x2_data, Y:y_data})
    if step % 20 == 0:
        print step, sess.run(cost, feed_dict={X1:x1_data, X2:x2_data, Y:y_data}), sess.run(w1), sess.run(w2), sess.run(b)

# Learns best fit is W: [1], b: [0]
print sess.run(hypothesis, feed_dict={X1:5, X2:4})
print sess.run(hypothesis, feed_dict={X1:2.5, X2:1.5})